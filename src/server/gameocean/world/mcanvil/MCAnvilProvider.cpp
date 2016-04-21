#include "MCAnvilProvider.h"

#include <string>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <zlib.h>
#include <gameocean/common.h>
#include <gameocean/utils/BinaryStream.h>
#include <gameocean/utils/CompressedBinaryStream.h>
#include <gameocean/world/Chunk.h>
#include <gameocean/world/ChunkPtr.h>
#include <gameocean/world/World.h>
#include <gameocean/world/tile/Tile.h>
#include "../../utils/NBT.h"

MCAnvilProvider::MCAnvilProvider(World& world) : ThreadedWorldProvider(world) {
    start();

    int fd = open(("worlds/" + world.getName() + "/level.dat").c_str(), O_RDONLY);
    if (fd < 0) {
        return;
    }
    std::unique_ptr<BinaryStream> s (new GzipInflateBinaryStream(std::unique_ptr<FileBinaryStream>(new FileBinaryStream(fd))));
    s->swapEndian = true;

    std::unique_ptr<NBTTag> ptag = NBTTag::getTag(*s);
    NBTCompound& tag = (NBTCompound&) *((NBTCompound&) *ptag).val["Data"];
    bool isTimeStopped = false;
    if (tag.val.count("GameRules") > 0) {
        NBTCompound& rules = (NBTCompound&) *tag.val["GameRules"];
        if (rules.val.count("doDaylightCycle") > 0)
            isTimeStopped = (((NBTString&) *rules.val["doDaylightCycle"]).val == "false");
    }
    world.setTime(((NBTInt&) *tag.val["Time"]).val, isTimeStopped);

    world.spawn = { ((NBTInt&) *tag.val["SpawnX"]).val, ((NBTInt&) *tag.val["SpawnY"]).val + 2, ((NBTInt&) *tag.val["SpawnZ"]).val };
}

void MCAnvilProvider::loadRegion(RegionPos pos) {
    std::stringstream ss;
    ss << "worlds/" << world.getName() << "/region/r." << pos.x << "." << pos.z << ".mca";
    int fd = open(ss.str().c_str(), O_RDONLY);
    if (fd < 0)
        return;
    Logger::main->trace("WorldProvider/MCAnvil", "Loading region: %s", ss.str().c_str());
    FileBinaryStream s (fd, true);
    
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

    ChunkPtr c;
    if (world.isChunkLoaded(pos)) {
        c = world.getChunkAt(pos, false);
    }
    if (!c)
        return;

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
    FileBinaryStream s (fd, true);
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
    std::unique_ptr<NBTTag> ptag = NBTTag::getTag(ms);
    NBTCompound& tag = (NBTCompound&) *((NBTCompound&) *ptag).val["Level"];

    c->clear();

    NBTList& list = (NBTList&) *tag.val["Sections"];
    for (std::unique_ptr<NBTTag>& sectionTag : list.val) {
        NBTCompound& section = (NBTCompound&) *sectionTag;

        char sectionY = ((NBTByte&) *section.val["Y"]).val;
        if (sectionY >= 8) continue;
        int offsetY = sectionY * 16 * 16 * 16;

        byte* ids = &((NBTByteArray&) *section.val["Blocks"]).val[0];
        NibbleArray<2048>* meta = (NibbleArray<2048>*) &((NBTByteArray&) *section.val["Data"]).val[0];
        NibbleArray<2048>* light = (NibbleArray<2048>*) &((NBTByteArray&) *section.val["BlockLight"]).val[0];
        NibbleArray<2048>* skylight = (NibbleArray<2048>*) &((NBTByteArray&) *section.val["SkyLight"]).val[0];
        /*
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
        }*/
        memcpy(&c->blockId[offsetY], &ids[0], 16 * 16 * 16);
        memcpy(&c->blockMeta.array[offsetY / 2], &meta[0], 16 * 16 * 16 / 2);
        memcpy(&c->blockLight.array[offsetY / 2], &light[0], 16 * 16 * 16 / 2);
        memcpy(&c->blockSkylight.array[offsetY / 2], &skylight[0], 16 * 16 * 16 / 2);
    }

    NBTIntArray& heightmap = (NBTIntArray&) *tag.val["HeightMap"];
    memcpy(&c->heightmap[0], &heightmap.val[0], heightmap.val.size() * 4);
    /*
    for (int i = 0; i < heightmap.val.size(); i++) {
        c->heightmap[i] = (byte) heightmap.val[i];
    }
*/

    if (tag.val.count("TileEntities") > 0) {
        c->tilesMutex.lock();
        NBTList& tileEntities = (NBTList&) *tag.val["TileEntities"];
        for (std::unique_ptr<NBTTag>& e : tileEntities.val) {
            NBTCompound& tileEntity = (NBTCompound&) *e;
            std::string entId = ((NBTString&) *tileEntity.val["id"]).val;
            int entX = ((NBTInt&) *tileEntity.val["x"]).val;
            int entY = ((NBTInt&) *tileEntity.val["y"]).val;
            int entZ = ((NBTInt&) *tileEntity.val["z"]).val;
            std::shared_ptr<Tile> tile = Tile::createTile(entId, world, { entX, entY, entZ });
            if (tile)
                c->tiles.insert(tile);
        }
        c->tilesMutex.unlock();
    }
    c->setLoaded();
}
