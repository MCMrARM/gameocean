#include "MCPEPacketBatchThread.h"

#include <iostream>
#include <zlib.h>
#include <RakNet/RakPeerInterface.h>
#include "MCPEProtocol.h"
#include "MCPEPlayer.h"

void MCPEPacketBatchThread::run() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(protocol.packetBatchDelay));
        if (stopping)
            return;

        std::map<RakNet::RakNetGUID, MCPEPlayer*> players = protocol.getPlayers();
        for (auto p : players) {
            MCPEPlayer* player = p.second;
            player->packetQueueMutex.lock();
            if (player->packetQueue.size() > 0) {
                if (player->packetQueue.size() == 1 &&
                        player->packetQueue.back().pk->id != MCPE_FULL_CHUNK_DATA_PACKET &&
                        player->packetQueue.back().pk->id != MCPE_UPDATE_BLOCK_PACKET &&
                        player->packetQueue.back().pk->id != MCPE_PLAYER_LIST_PACKET) {
                    auto p = player->packetQueue.back();
                    MCPEPacket* pk = p.pk;
                    int ret = player->directPacket(pk);
                    p.callback(player, pk, ret);
                    delete pk;
                    player->packetQueue.clear();
                    player->packetQueueMutex.unlock();
                } else {
                    std::deque<MCPEPlayer::QueuedPacket> packetQueue (player->packetQueue);
                    player->packetQueue.clear();
                    player->packetQueueMutex.unlock();

                    RakNet::BitStream bs;
                    bs.Write((RakNet::MessageID) 0x8e);
                    bs.Write((RakNet::MessageID) MCPE_BATCH_PACKET);
                    bs.Write((int) 0);

                    const int BUFFER_SIZE = 1024 * 16;
                    byte data [BUFFER_SIZE];

                    z_stream zs;
                    zs.opaque = Z_NULL;
                    zs.zfree = Z_NULL;
                    zs.zalloc = Z_NULL;

                    int ret = deflateInit(&zs, 7);
                    if (ret != Z_OK) {
                        deflateEnd(&zs);
                        Logger::main->error("MCPE/Batch", "Failed to batch packet");
                        continue;
                    }

                    bool needsACK = false;
                    auto it = packetQueue.begin();
                    while (it != packetQueue.end()) {
                        MCPEPacket* pk = it->pk;
                        if (pk->needsACK)
                            needsACK = true;
                        RakNet::BitStream pbs;
                        pbs.Write((int) 0);
                        pbs.Write((RakNet::MessageID) pk->id);
                        pk->write(pbs);

                        RakNet::BitSize_t o = pbs.GetWriteOffset();
                        pbs.SetWriteOffset(0);
                        pbs.Write(BITS_TO_BYTES(o) - 4);
                        pbs.SetWriteOffset(o);

                        it++;
                        int flush = (it == packetQueue.end() ? Z_FINISH : Z_NO_FLUSH);

                        zs.avail_in = pbs.GetNumberOfBytesUsed();
                        zs.next_in = &pbs.GetData()[0];

                        do {
                            zs.avail_out = BUFFER_SIZE;
                            zs.next_out = &data[0];
                            ret = deflate(&zs, flush);
                            if (ret == Z_STREAM_ERROR) {
                                Logger::main->error("MCPE/Batch", "Failed to batch packet");
                                deflateEnd(&zs);
                                break;
                            }
                            bs.Write((char*) data, BUFFER_SIZE - zs.avail_out);
                        } while (zs.avail_out == 0);

                        if (zs.avail_in != 0)
                            Logger::main->error("MCPE/Batch", "Failed to batch packet");
                    }
                    deflateEnd(&zs);
                    int o = bs.GetWriteOffset();
                    bs.SetWriteOffset(16);
                    bs.Write(BITS_TO_BYTES(o) - 6);
                    bs.SetWriteOffset(o);

                    int i = protocol.getPeer()->Send(&bs, MEDIUM_PRIORITY, needsACK ? RELIABLE_WITH_ACK_RECEIPT : RELIABLE, 0, player->address, false);
                    for (MCPEPlayer::QueuedPacket& p : packetQueue) {
                        p.callback(player, p.pk, i);
                        delete p.pk;
                    }
                }
            } else {
                player->packetQueueMutex.unlock();
            }
        }
    }
}