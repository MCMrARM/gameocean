#include "MCPEPlayer.h"

#include <iostream>
#include <string>
#include <RakNet/RakPeerInterface.h>
#include "MCPEPacket.h"
#include "../../world/World.h"
#include "../../world/Chunk.h"

int MCPEPlayer::writePacket(MCPEPacket &packet) {
    RakNet::BitStream bs;
    bs.Write((RakNet::MessageID) packet.id);
    packet.write(bs);
    return this->protocol.getPeer()->Send(&bs, MEDIUM_PRIORITY, packet.reliable ? (packet.needsACK ? RELIABLE_WITH_ACK_RECEIPT : RELIABLE) : UNRELIABLE, 0, address, false);
}

bool MCPEPlayer::sendChunk(int x, int z) {
    if (!Player::sendChunk(x, z)) return false;
    Chunk* chunk = this->world.getChunkAt(x, z, true);
    if (chunk == null) return false;

    MCPEFullChunkDataPacket pk;
    pk.chunk = chunk;
    pk.needsACK = true;
    int id = writePacket(pk);

    raknetChunkQueue[id] = ChunkPos(x, z);
    return true;
}

void MCPEPlayer::receivedChunk(int x, int z) {
    Player::receivedChunk(x, z);

    chunkArrayMutex.lock();
    if (!spawned && receivedChunks.size() > 54) {
        spawned = true;

        MCPERespawnPacket pk;
        pk.x = x;
        pk.y = y;
        pk.z = z;
        writePacket(pk);

        MCPEPlayStatusPacket pk2;
        pk2.status = MCPEPlayStatusPacket::Status::PLAYER_SPAWN;
        writePacket(pk2);
    }
    chunkArrayMutex.unlock();
}

void MCPEPlayer::receivedACK(int packetId) {
    if (raknetChunkQueue.count(packetId) > 0) {
        ChunkPos pos = raknetChunkQueue.at(packetId);
        MCPEPlayer::receivedChunk(pos.x, pos.z);
    }
}

void MCPEPlayer::sendMessage(std::string text) {
    MCPETextPacket pk;
    pk.type = MCPETextPacket::MessageType::RAW;
    pk.message = text.c_str();
    writePacket(pk);
}

void MCPEPlayer::sendPosition(float x, float y, float z) {
    MCPEMovePlayerPacket pk;
    pk.eid = 0;
    pk.x = x;
    pk.y = y;
    pk.z = z;
    pk.mode = MCPEMovePlayerPacket::Mode::RESET;
    writePacket(pk);
}