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
        setSpawned();
    }
    chunkArrayMutex.unlock();
}

void MCPEPlayer::setSpawned() {
    if (spawned)
        return;

    Player::setSpawned();

    MCPERespawnPacket pk;
    pk.x = x;
    pk.y = y;
    pk.z = z;
    writePacket(pk);

    MCPEPlayStatusPacket pk2;
    pk2.status = MCPEPlayStatusPacket::Status::PLAYER_SPAWN;
    writePacket(pk2);
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

void MCPEPlayer::spawnEntity(Entity *entity) {
    Player::spawnEntity(entity);

    if (closed)
        return;
    if (entity->getTypeName() == Player::TYPE_NAME) {
        Logger::main->debug("MCPE/Player", "Spawning player: %s", ((Player*) entity)->getName().c_str());
        unsigned char skin[64 * 32 * 4];

        UUID uuid = {1, entity->getId()};

        MCPEPlayerListPacket lpk;
        lpk.type = MCPEPlayerListPacket::Type::ADD;
        lpk.addEntries.push_back({uuid, entity->getId(), ((Player*) entity)->getName().c_str(), false, &skin[0]});
        writePacket(lpk);

        MCPEAddPlayerPacket pk;
        pk.uuid = uuid;
        pk.eid = entity->getId();
        pk.username = ((Player*) entity)->getName().c_str();
        Vector3D v = entity->getPos();
        pk.x = v.x;
        pk.y = v.y;
        pk.z = v.z;
        pk.yaw = pk.headYaw = 0;
        pk.pitch = 0;
        writePacket(pk);

        updateEntityPos(entity);
        return;
    }
}
void MCPEPlayer::despawnEntity(Entity *entity) {
    Player::despawnEntity(entity);
    if (closed)
        return;
    if (entity->getTypeName() == Player::TYPE_NAME) {
        Logger::main->debug("MCPE/Player", "Despawning player: %s", ((Player*) entity)->getName().c_str());

        UUID uuid = {1, entity->getId()};

        MCPERemovePlayerPacket pk;
        pk.uuid = uuid;
        pk.eid = entity->getId();
        writePacket(pk);

        MCPEPlayerListPacket lpk;
        lpk.type = MCPEPlayerListPacket::Type::REMOVE;
        lpk.removeEntries.push_back({uuid});
        writePacket(lpk);
        return;
    }
}

void MCPEPlayer::updateEntityPos(Entity *entity) {
    MCPEMoveEntityPacket pk; // TODO: Batch
    Vector3D pos = entity->getPos();
    pk.entries.push_back({entity->getId(), pos.x, pos.y, pos.z, 0, 0, 0});
    writePacket(pk);
}