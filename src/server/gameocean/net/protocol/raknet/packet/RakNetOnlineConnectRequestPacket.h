#pragma once

#include <gameocean/net/Packet.h>
#include "../RakNetPacketIds.h"
#include "../RakNetProtocol.h"

struct RakNetOnlineConnectRequestPacket : public Packet {
    PacketDef(RAKNET_PACKET_ONLINE_CONNECT_REQUEST, Type::SERVERBOUND)

    long long guid;
    long long time;
    char hasSecurity = 0;

    RakNetOnlineConnectRequestPacket() {
    }

    virtual unsigned int getPacketSize() const {
        return 2 * BinaryStream::LONG_SIZE;
    }

    virtual void read(BinaryStream& stream) {
        stream >> guid >> time >> hasSecurity;
    }

    virtual void write(BinaryStream& stream) {
        stream << guid << time << hasSecurity;
    }

    virtual void handleServer(Connection &connection);
};