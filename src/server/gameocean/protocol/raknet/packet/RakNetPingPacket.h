#pragma once

#include <gameocean/net/Packet.h>
#include "../RakNetPacketIds.h"
#include "../RakNetProtocol.h"

struct RakNetPingPacket : public Packet {
    PacketDef(RAKNET_PACKET_PING, Type::BOTH)

    long long time;

    virtual unsigned int getPacketSize() const {
        return BinaryStream::LONG_SIZE;
    }

    virtual void read(BinaryStream& stream) {
        stream >> time;
    }

    virtual void write(BinaryStream& stream) {
        stream << time;
    }

    virtual void handleServer(Connection &connection);
    virtual void handleClient(Connection &connection) { handleServer(connection); }
};