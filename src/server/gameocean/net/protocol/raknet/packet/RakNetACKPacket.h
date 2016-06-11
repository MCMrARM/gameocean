#pragma once

#include <gameocean/net/Packet.h>
#include "../RakNetPacketIds.h"
#include "../RakNetProtocol.h"
#include "RakNetBaseACKPacket.h"

struct RakNetACKPacket : public RakNetBaseACKPacket {

    PacketDef(RAKNET_PACKET_ACK, Type::BOTH)

    virtual void handleServer(Connection &connection);

};