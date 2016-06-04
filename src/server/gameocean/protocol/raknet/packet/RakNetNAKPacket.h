#pragma once

#include <gameocean/net/Packet.h>
#include "../RakNetPacketIds.h"
#include "../RakNetProtocol.h"
#include "RakNetBaseACKPacket.h"

struct RakNetNAKPacket : public RakNetBaseACKPacket {

    PacketDef(RAKNET_PACKET_NAK, Type::BOTH)

    virtual void handleServer(Connection &connection);

};