#include "MCPEPlayer.h"

#include "../../libs/RakNet/RakPeerInterface.h"
#include "MCPEPacket.h"

void MCPEPlayer::writePacket(MCPEPacket &packet) {
    RakNet::BitStream bs;
    bs.Write((RakNet::MessageID) packet.id);
    packet.write(bs);
    this->protocol.getPeer()->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, (char) packet.channel, address, false);
}