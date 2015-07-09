#include "MCPEProtocol.h"

#include "common.h"
#include <zlib.h>
#include "../../libs/RakNet/RakPeerInterface.h"
#include "../../libs/RakNet/MessageIdentifiers.h"
#include "../../libs/RakNet/BitStream.h"
#include "../../libs/RakNet/RakNetTypes.h"
#include "MCPEPacket.h"
#include "MCPEPlayer.h"

void MCPEProtocol::bind(int port) {
    peer = RakNet::RakPeerInterface::GetInstance();
    RakNet::SocketDescriptor sd(port,0);
    peer->Startup(server.maxPlayers + 1, &sd, 1);
    peer->SetMaximumIncomingConnections(server.maxPlayers + 1);

    MCPEPacket::registerPackets();
}

void MCPEProtocol::loop() {
    RakNet::Packet* packet;
    while (1) {
        for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
            processPacket(packet);
        }
    }
}

void MCPEProtocol::processPacket(RakNet::Packet *packet) {
    switch (packet->data[0])
    {
        case ID_REMOTE_CONNECTION_LOST:
        case ID_DISCONNECTION_NOTIFICATION:
        {
            printf("* connection lost\n");
            if (players.count(packet->guid) > 0) {
                MCPEPlayer* player = players.at(packet->guid);
                players.erase(packet->guid);
                server.removePlayer(player);
            }
        }
            break;
        case ID_NEW_INCOMING_CONNECTION: {
            Logger::main->info("MCPE/Connection", "A new client has connected! %s", packet->systemAddress.ToString(true, ':'));
            MCPEPlayer *player = new MCPEPlayer(this->server, *this, packet->guid, packet->systemAddress);
            players[packet->guid] = player;
            server.addPlayer(player);
        }
            break;
        case ID_SND_RECEIPT_ACKED:
        {
            int msgId = *(int*)(&packet->data[1]);
            if (players.count(packet->guid) <= 0) {
                break;
            }
            MCPEPlayer* p = players.at(packet->guid);
            p->receivedACK(msgId);
        }
            break;
        case MCPE_BATCH_PACKET:
        {
            RakNet::BitStream bs(packet->data, packet->length, false);
            bs.IgnoreBytes(sizeof(RakNet::MessageID));
            int size;
            bs.Read(size);
            char* data = new char[size];
            bs.Read(data, size);

            int bufSize = 64 * 1024 * 1024;
            byte* decompressed = new byte[bufSize];
            z_stream zs;
            zs.opaque = Z_NULL;
            zs.zfree = Z_NULL;
            zs.zalloc = Z_NULL;
            zs.avail_in = size;
            zs.avail_out = bufSize;
            zs.next_in = (unsigned char*) data;
            zs.next_out = decompressed;

            int ret = inflateInit(&zs);
            if(ret != Z_OK) {
                inflateEnd(&zs);
                delete[] data;
                delete[] decompressed;
                break;
            }
            ret = inflate(&zs, Z_FINISH);
            delete[] data;
            if(ret != Z_STREAM_END) {
                inflateEnd(&zs);
                delete[] decompressed;
                break;
            }
            inflateEnd(&zs);

            RakNet::BitStream dbs(decompressed, zs.total_out, false);

            byte pkId;

            while (dbs.GetNumberOfUnreadBits() > 0) {
                dbs.Read(pkId);

                MCPEPacket* pk = MCPEPacket::getPacket(pkId);
                if (pk != null) {
                    pk->read(dbs);
                    this->handlePacket(packet, *pk);
                    delete pk;
                } else {
                    Logger::main->warn("MCPE/BatchPacket", "Unknown packet id: %i", pkId);
                    break;
                }
            }

            delete[] decompressed;
        }
            break;
        default:
            MCPEPacket* pk = MCPEPacket::getPacket(packet->data[0]);
            if (pk != null) {
                RakNet::BitStream bs(packet->data, packet->length, false);
                bs.IgnoreBytes(sizeof(RakNet::MessageID));
                pk->read(bs);
                this->handlePacket(packet, *pk);
                delete pk;
            } else {
                Logger::main->debug("MCPE/Packet", "Unknown packet id: %i", packet->data[0]);
            }
            break;
    }
}

void MCPEProtocol::handlePacket(RakNet::Packet *raknetPacket, MCPEPacket &packet) {
    if (players.count(raknetPacket->guid) <= 0) {
        Logger::main->debug("MCPE/Packet", "A packet was sent but there isn't a Player instance for it!");
        return;
    }
    MCPEPlayer* p = players.at(raknetPacket->guid);
    packet.handle(*p);
}

