#pragma once

#include <gameocean/net/Packet.h>
#include "../RakNetPacketIds.h"
#include "../RakNetProtocol.h"

struct RakNetDisconnectPacket : public Packet {
    PacketDef(RAKNET_PACKET_DISCONNECT, Type::BOTH)

    virtual unsigned int getPacketSize() const {
        return 0;
    }

    virtual void read(BinaryStream& stream) { }

    virtual void write(BinaryStream& stream) { }

    virtual void handleServer(Connection &connection);
};