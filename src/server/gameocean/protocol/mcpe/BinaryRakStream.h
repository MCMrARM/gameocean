#pragma once

#include <gameocean/utils/BinaryStream.h>
#include <RakNet/BitStream.h>

class BinaryRakStream : public BinaryStream {

public:
    RakNet::BitStream& stream;

    BinaryRakStream(RakNet::BitStream& stream) : stream(stream) {
        //
    }

    virtual unsigned int read(byte *data, unsigned int size) {
        stream.Read((char*) data, size);
    }

    virtual void write(const byte *data, unsigned int size) {
        stream.Write((const char*) data, size);
    }

};