#include "MCPEProtocol.h"

#include <gameocean/common.h>
#include <zlib.h>
#include <sstream>
#include <chrono>
#include <iostream>
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>
#include "MCPEPacket.h"
#include "MCPEPlayer.h"
#include "MCPEPacketBatchThread.h"

void MCPEProtocol::start() {
    peer = RakNet::RakPeerInterface::GetInstance();
    std::string s = "MCPE;A Test Server;34;0.12.1;0;20";
    peer->SetOfflinePingResponse(s.c_str(), s.length() + 1);
    RakNet::SocketDescriptor sd(port, 0);
    peer->Startup(server.maxPlayers + 1, &sd, 1);
    peer->SetMaximumIncomingConnections(server.maxPlayers + 1);

    MCPEPacket::registerPackets();

    batchThread.start();
    Protocol::start();
}

void MCPEProtocol::stop() {
    batchThread.stop();
    Protocol::stop();
}

void MCPEProtocol::loop() {
    RakNet::Packet* packet;
    while (true) {
        if (shouldStop)
            break;

        for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
            processPacket(packet);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void MCPEProtocol::processPacket(RakNet::Packet *packet) {
    switch (packet->data[0])
    {
        case ID_UNCONNECTED_PING:
        case ID_UNCONNECTED_PING_OPEN_CONNECTIONS:
            break;
        case ID_REMOTE_CONNECTION_LOST:
        case ID_DISCONNECTION_NOTIFICATION:
        case ID_CONNECTION_LOST:
        {
            printf("* connection lost\n");
            if (players.count(packet->guid) > 0) {
                MCPEPlayer* player = players.at(packet->guid);
                player->close("connection lost", false);
                playersMutex.lock();
                players.erase(packet->guid);
                playersMutex.unlock();
                server.removePlayer(player);
                delete player;
            }
        }
            break;
        case ID_NEW_INCOMING_CONNECTION: {
            Logger::main->info("MCPE/Connection", "A new client has connected! %s", packet->systemAddress.ToString(true, ':'));
            MCPEPlayer *player = new MCPEPlayer(this->server, *this, packet->guid, packet->systemAddress);
            playersMutex.lock();
            players[packet->guid] = player;
            playersMutex.unlock();
            server.addPlayer(player);
        }
            break;
        case ID_SND_RECEIPT_ACKED:
        {
            int msgId = *(int*)(&packet->data[1]);
            if (players.count(packet->guid) <= 0) {
                break;
            }
            playersMutex.lock();
            MCPEPlayer* p = players.at(packet->guid);
            playersMutex.unlock();
            p->receivedACK(msgId);
        }
            break;
        case MCPE_BATCH_PACKET:
        {
            playersMutex.lock();
            if (players.count(packet->guid) <= 0) {
                Logger::main->debug("MCPE/BatchPacket", "Packet was sent from a client that's not in-game!");
                playersMutex.unlock();
                return;
            }
            MCPEPlayer* p = players.at(packet->guid);
            playersMutex.unlock();

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

            int pkSize;
            byte pkId;

            while (dbs.GetNumberOfUnreadBits() > 0) {
                dbs.Read(pkSize);
                dbs.Read(pkId);
                Logger::main->trace("MCPE/BatchPacket", "Reading: %i", pkId);

                MCPEPacket* pk = MCPEPacket::getPacket(pkId);
                if (pk != null) {
                    int s = BITS_TO_BYTES(dbs.GetReadOffset());
                    pk->read(dbs);
                    int used = BITS_TO_BYTES(dbs.GetReadOffset()) - s;
                    if (used != pkSize - 1) {
                        Logger::main->debug("MCPE/BatchPacket", "Read %i instead of %i bytes (packet id: %i)", used, pkSize, pkId);
                        delete pk;
                        break;
                    }

                    pk->handle(*p);
                    delete pk;
                } else {
                    Logger::main->debug("MCPE/BatchPacket", "Unknown packet id: %i", pkId);
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

                playersMutex.lock();
                if (players.count(packet->guid) <= 0) {
                    Logger::main->debug("MCPE/Packet", "Packet was sent from a client that's not in-game!");
                    playersMutex.unlock();
                    return;
                }
                MCPEPlayer* p = players.at(packet->guid);
                playersMutex.unlock();
                pk->handle(*p);
                delete pk;
            } else {
                Logger::main->debug("MCPE/Packet", "Unknown packet id: %i", packet->data[0]);
            }
            break;
    }
}

