#pragma once

#include <gameocean/net/Packet.h>
#include "../RakNetPacketIds.h"
#include "../RakNetProtocol.h"

struct RakNetDetectLostConnectionsPacket : public Packet {
    PacketDef(RAKNET_PACKET_DETECT_LOST_CONNECTION, Type::BOTH)

    virtual unsigned int getPacketSize() const {
        return 0;
    }

    virtual void read(BinaryStream& stream) { }

    virtual void write(BinaryStream& stream) { }
};