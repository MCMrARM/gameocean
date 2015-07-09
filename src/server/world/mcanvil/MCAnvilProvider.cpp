#include "MCAnvilProvider.h"

#include <string>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <zlib.h>
#include "common.h"
#include <utils/BinaryStream.h>
#include "../Chunk.h"
#include "../World.h"
#include "../../utils/NBT.h"

void MCAnvilProvider::loadRegion(RegionPos pos) {
    std::stringstream ss;
    ss << "worlds/world/region/r." << pos.x << "." << pos.z << ".mca";
    int fd = open(ss.str().c_str(), O_RDONLY);
    if (fd < 0)
        return;
    Logger::main->trace("WorldProvider/MCAnvil", "Loading region: %s", ss.str().c_str());
    FileBinaryStream s (fd);
    
    std::shared_ptr<MCAnvilHeader> header (new MCAnvilHeader());
    header->filePath = ss.str();
    s.read((byte*) header->locations, sizeof(header->locations));
    s.read((byte*) header->timestamps, sizeof(header->timestamps));
    regions[pos] = header;
}

void MCAnvilProvider::loadChunk(ChunkPos pos) {
    RegionPos r (pos.x >> 5, pos.z >> 5);
    if (regions.count(r) <= 0)
        loadRegion(r);

    Chunk* c = null;
    if (world.isChunkLoaded(pos)) {
        c = world.getChunkAt(pos, false);
    }
    if (c == null) {
        c = new Chunk(pos);
        world.setChunk(c);
    }

    if (regions.count(r) <= 0) { // the region may not exist
        c->clear();
        c->ready = true;
        return;
    }

    std::shared_ptr<MCAnvilHeader> header = regions.at(r);
    int off = ((pos.z & 31) << 5) | (pos.x & 31);
    unsigned int loc = header->locations[off];
    BinaryStream::swapBytes((byte*) &loc, 4);
    loc = loc >> 8;
    if (loc == 0) {
        c->clear();
        c->ready = true;
        return;
    }

    int fd = open(header->filePath.c_str(), O_RDONLY);
    if (fd < 0) { // this shouldn't happen but let's make sure
        c->clear();
        c->ready = true;
        return;
    }
    lseek(fd, loc << 12, SEEK_SET);
    FileBinaryStream s (fd);
    s.swapEndian = true;
    unsigned int byteSize;
    byte compressionType;
    s >> byteSize >> compressionType;
    byteSize--;
    byte* data = new byte[byteSize];
    if (compressionType == 2) {
        s.read(data, byteSize);

        int decompressChunkSize = 4096;
        int decompressBufferSize = decompressChunkSize;
        byte* decompressed = new byte[decompressBufferSize];
        z_stream zs;
        zs.opaque = Z_NULL;
        zs.zfree = Z_NULL;
        zs.zalloc = Z_NULL;
        zs.avail_in = byteSize;
        zs.avail_out = decompressBufferSize;
        zs.next_in = data;
        zs.next_out = decompressed;

        int ret = Z_DATA_ERROR;
        ret = inflateInit(&zs);
        if(ret != Z_OK) {
            Logger::main->warn("WorldProvider/MCAnvil", "Failed to decompress chunk");
            c->ready = true;
            inflateEnd(&zs);
            delete[] decompressed;
            delete[] data;
            return;
        }

        ret = Z_OK;
        while (ret != Z_STREAM_END) {
            ret = inflate(&zs, Z_NO_FLUSH);

            if (ret == Z_OK && zs.avail_out == 0) {
                // grow the buffer
                byte* _decompressed = decompressed;
                int oldBufferSize = decompressBufferSize;
                decompressBufferSize *= 2;
                decompressed = new byte[decompressBufferSize];
                memcpy(decompressed, _decompressed, oldBufferSize);
                delete[] _decompressed;
                zs.avail_out = decompressBufferSize - oldBufferSize;
                zs.next_out = &decompressed[oldBufferSize];
            } else if (ret != Z_STREAM_END) {
                Logger::main->warn("WorldProvider/MCAnvil", "Failed to decompress chunk");
                c->ready = true;
                inflateEnd(&zs);
                delete[] data;
                return;
            }
        }

        delete[] data;
        data = decompressed;
        byteSize = decompressBufferSize - zs.avail_out;
    } else {
        Logger::main->warn("WorldProvider/MCAnvil", "Unsupported compression type: %i", compressionType);
        c->ready = true;
        delete[] data;
        return;
    }

    MemoryBinaryStream ms (data, byteSize);
    ms.swapEndian = true;
    NBTCompound* tag = (NBTCompound*) NBTTag::getTag(ms);
    tag = (NBTCompound*) tag->val["Level"];

    c->clear();

    NBTList* list = (NBTList*) tag->val["Sections"];
    for (NBTTag* sectionTag : list->val) {
        NBTCompound* section = (NBTCompound*) sectionTag;

        char sectionY = ((NBTByte*) section->val["Y"])->val;
        if (sectionY >= 8) continue;
        int offsetY = sectionY * 16;

        byte* ids = ((NBTByteArray*) section->val["Blocks"])->val;
        NibbleArray<2048>* meta = (NibbleArray<2048>*) ((NBTByteArray*) section->val["Data"])->val;
        NibbleArray<2048>* light = (NibbleArray<2048>*) ((NBTByteArray*) section->val["BlockLight"])->val;
        NibbleArray<2048>* skylight = (NibbleArray<2048>*) ((NBTByteArray*) section->val["SkyLight"])->val;
        int i = 0;
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
                for (int x = 0; x < 16; x++) {
                    int j = c->getBlockPos(x, offsetY + y, z);
                    c->blockId[j] = ids[i];
                    c->blockMeta.set(j, (*meta)[i]);
                    c->blockLight.set(j, (*light)[i]);
                    c->blockSkylight.set(j, (*skylight)[i]);
                    i++;
                }
            }
        }
    }

    NBTIntArray* heightmap = (NBTIntArray*) tag->val["HeightMap"];
    for (int i = 0; i < heightmap->val.size(); i++) {
        c->heightmap[i] = (byte) heightmap->val[i];
    }
    c->ready = true;
    delete tag;
}
