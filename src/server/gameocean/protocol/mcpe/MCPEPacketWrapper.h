#pragma once

#include <gameocean/net/Packet.h>
#include <gameocean/utils/BinaryStream.h>
#include "MCPEPacket.h"

#define MCPE_WRAPPER_PACKET_ID 0x8e

class MCPESendDataPacketWrapper : public Packet {
public:
    PacketDef(MCPE_WRAPPER_PACKET_ID, Type::CLIENTBOUND)
    DynamicMemoryBinaryStream stream;

    MCPESendDataPacketWrapper() {
        stream.swapEndian = true;
    }

    virtual unsigned int getPacketSize() const {
        return this->stream.getSize();
    }

    virtual void read(BinaryStream &stream) { }
    virtual void write(BinaryStream &stream) {
        stream.write(this->stream.getBuffer(false), this->stream.getSize());
    }
};

class MCPESendPacketWrapper : public MCPESendDataPacketWrapper {
public:
    PacketDef(MCPE_WRAPPER_PACKET_ID, Type::CLIENTBOUND)
    MCPEPacket &packet;

    MCPESendPacketWrapper(MCPEPacket &packet) : packet(packet) {
        stream << (byte) packet.id;
        packet.write(stream); // TODO: Optimize this!
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