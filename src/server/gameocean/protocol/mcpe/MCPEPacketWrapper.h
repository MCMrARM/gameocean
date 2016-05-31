#pragma once

#include <gameocean/net/Packet.h>
#include <gameocean/utils/BinaryStream.h>
#include "MCPEPacket.h"

#define MCPE_WRAPPER_PACKET_ID 0x8e

class MCPESendPacketWrapper : public Packet {
public:
    PacketDef(MCPE_WRAPPER_PACKET_ID, Type::CLIENTBOUND)
    MCPEPacket &packet;
    DynamicMemoryBinaryStream stream;

    MCPESendPacketWrapper(MCPEPacket &packet) : packet(packet) {
        packet.write(stream); // TODO: Optimize this!
    }

    virtual unsigned int getPacketSize() const {
        return this->stream.getSize();
    }

    virtual void read(BinaryStream &stream) { }
    virtual void write(BinaryStream &stream) {
        stream.write(this->stream.getBuffer(false), this->stream.getSize());
    }
};

class MCPEReceivePacketWrapper : public Packet {
public:
    PacketDef(MCPE_WRAPPER_PACKET_ID, Type::SERVERBOUND)

    MCPEPacket *recvdPk = nullptr;

    virtual ~MCPEReceivePacketWrapper() {
        if (recvdPk != nullptr)
            delete recvdPk;
    }

    virtual unsigned int getPacketSize() const {
        return 0; // we will not use this function
    }

    virtual void read(BinaryStream &stream);
    virtual void write(BinaryStream &stream) { }

    virtual void handleServer(Connection &connection);
};