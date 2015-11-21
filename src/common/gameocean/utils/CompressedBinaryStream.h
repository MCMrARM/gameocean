#pragma once

#include <zlib.h>
#include "BinaryStream.h"

class ZlibInflateBinaryStream : public WrapperBinaryStream {

protected:
    static const unsigned int CHUNK_SIZE = 1024 * 1024;
    byte input [CHUNK_SIZE];
    int inputPos = 0;
    z_stream zs;
    virtual void initZlib();
    void feedInput();

public:
    ZlibInflateBinaryStream(std::unique_ptr<BinaryStream> stream);

    virtual void write(const byte *data, unsigned int size);

    virtual unsigned int read(byte *data, unsigned int size);

};

class GzipInflateBinaryStream : public ZlibInflateBinaryStream {

protected:
    virtual void initZlib();

public:
    GzipInflateBinaryStream(std::unique_ptr<BinaryStream> stream) : ZlibInflateBinaryStream(std::move(stream)) {

    };

};