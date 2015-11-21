#include "CompressedBinaryStream.h"

ZlibInflateBinaryStream::ZlibInflateBinaryStream(std::unique_ptr<BinaryStream> stream) : WrapperBinaryStream(std::move(stream)) {
    zs.opaque = Z_NULL;
    zs.zfree = Z_NULL;
    zs.zalloc = Z_NULL;
    zs.avail_in = 0;
    zs.avail_out = 0;
    zs.next_in = Z_NULL;
    zs.next_out = Z_NULL;

    feedInput();
}

void ZlibInflateBinaryStream::feedInput() {
    try {
        inputPos = 0;
        this->stream->read(&input[0], CHUNK_SIZE);
    } catch (EOFException exception) {
    }
}

void ZlibInflateBinaryStream::initZlib() {
    int ret = inflateInit(&zs);
    if (ret != Z_OK) {
        Logger::main->warn("ZlibInflateBinaryStream", "inflateInit failed");
        return;
    }
}

unsigned int ZlibInflateBinaryStream::read(byte* data, unsigned int size) {
    if (zs.next_in == Z_NULL) {
        initZlib();
    }

    zs.avail_in = CHUNK_SIZE - inputPos;
    zs.next_in = &input[inputPos];
    zs.avail_out = size;
    zs.next_out = &data[0];

    int ret = Z_OK;
    while (ret != Z_STREAM_END) {
        ret = inflate(&zs, Z_NO_FLUSH);

        if (zs.avail_out > 0 && zs.avail_in == 0) {
            feedInput();
            zs.avail_in = CHUNK_SIZE;
            zs.next_in = &input[0];
        } else if (zs.avail_out == 0 && zs.avail_in > 0) {
            inputPos = CHUNK_SIZE - zs.avail_in;
            return (size - zs.avail_out);
        } else {
            Logger::main->warn("ZlibInflateBinaryStream", "Failed to inflate %i %i", zs.avail_in, zs.avail_out);
            Logger::main->warn("ZlibInflateBinaryStream", "Failed to inflate");
            return (size - zs.avail_out);
        }
    }
}

void ZlibInflateBinaryStream::write(const byte* data, unsigned int size) {
    Logger::main->warn("ZlibInflateBinaryStream", "Write called");
}

void GzipInflateBinaryStream::initZlib() {
    int ret = inflateInit2(&zs, 16 + MAX_WBITS);
    if (ret != Z_OK) {
        Logger::main->warn("GzipInflateBinaryStream", "inflateInit failed");
        return;
    }
}