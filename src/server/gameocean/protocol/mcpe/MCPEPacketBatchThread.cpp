#include "MCPEPacketBatchThread.h"

#include <iostream>
#include <zlib.h>
#include "MCPEProtocolServer.h"
#include "MCPEPlayer.h"
#include "MCPEPacketWrapper.h"

void MCPEPacketBatchThread::run() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(protocol.packetBatchDelay));
        if (stopping)
            return;

        auto players = protocol.getPlayers();
        for (auto& player : players) {
            player->packetQueueMutex.lock();
            if (player->packetQueue.size() > 0) {
                if (player->packetQueue.size() == 1 &&
                        player->packetQueue.back().pk->id != MCPE_FULL_CHUNK_DATA_PACKET &&
                        player->packetQueue.back().pk->id != MCPE_UPDATE_BLOCK_PACKET &&
                        player->packetQueue.back().pk->id != MCPE_PLAYER_LIST_PACKET) {
                    auto pk = std::move(player->packetQueue.back());
                    int ret = player->directPacket(pk.pk.get());
                    pk.callback(pk.pk.get(), ret);
                    player->packetQueue.clear();
                    player->packetQueueMutex.unlock();
                } else {
                    std::deque<MCPEPlayer::QueuedPacket> packetQueue = std::move(player->packetQueue);
                    player->packetQueueMutex.unlock();

                    MCPESendDataPacketWrapper wrapper;
                    DynamicMemoryBinaryStream &bs = wrapper.stream;
                    bs.swapEndian = true;
                    //bs << (byte) 0x8e;
                    bs << (byte) MCPE_BATCH_PACKET;
                    bs << (int) 0;

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
                        MCPEPacket* pk = it->pk.get();
                        if (pk->needsACK)
                            needsACK = true;
                        DynamicMemoryBinaryStream pbs;
                        pbs.swapEndian = true;
                        pbs << (int) 0 << (byte) pk->id;
                        pk->write(pbs);

                        ((int*) pbs.getBuffer(false))[0] = pbs.getSize() - 4;
                        BinaryStream::swapBytes(&pbs.getBuffer(false)[0], sizeof(int));

                        it++;
                        int flush = (it == packetQueue.end() ? Z_FINISH : Z_NO_FLUSH);

                        zs.avail_in = pbs.getSize();
                        zs.next_in = &pbs.getBuffer(false)[0];

                        do {
                            zs.avail_out = BUFFER_SIZE;
                            zs.next_out = &data[0];
                            ret = deflate(&zs, flush);
                            if (ret == Z_STREAM_ERROR) {
                                Logger::main->error("MCPE/Batch", "Failed to batch packet");
                                deflateEnd(&zs);
                                break;
                            }
                            bs.write(data, BUFFER_SIZE - zs.avail_out);
                        } while (zs.avail_out == 0);

                        if (zs.avail_in != 0)
                            Logger::main->error("MCPE/Batch", "Failed to batch packet");
                    }
                    deflateEnd(&zs);
                    ((int*) (&bs.getBuffer(false)[1]))[0] = bs.getSize() - 5;
                    BinaryStream::swapBytes(&bs.getBuffer(false)[1], sizeof(int));

                    int i = player->getConnection().send(wrapper, (needsACK ? RakNetReliability::RELIABLE_ACK_RECEIPT : RakNetReliability::RELIABLE));
                    for (MCPEPlayer::QueuedPacket& pk : packetQueue) {
                        pk.callback(pk.pk.get(), i);
                    }
                }
            } else {
                player->packetQueueMutex.unlock();
            }
        }
    }
}