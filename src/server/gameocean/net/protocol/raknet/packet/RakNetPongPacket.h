#pragma once

#include <gameocean/net/Packet.h>
#include "../RakNetPacketIds.h"
#include "../RakNetProtocol.h"

struct RakNetPongPacket : public Packet {
    PacketDef(RAKNET_PACKET_PONG, Type::BOTH)

    long long pingTime, time;

    virtual unsigned int getPacketSize() const {
        return 2 * BinaryStream::LONG_SIZE;
    }

    virtual void read(BinaryStream& stream) {
        stream >> pingTime >> time;
    }

    virtual void write(BinaryStream& stream) {
        stream << pingTime << time;
    }

    virtual void handleServer(Connection &connection);
};