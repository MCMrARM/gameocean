#include "MCPEPlayer.h"

#include <iostream>
#include <string>
#include <RakNet/RakPeerInterface.h>
#include "MCPEPacket.h"
#include "../../world/World.h"
#include "../../world/Chunk.h"


void MCPEPlayer::batchPacketCallback(std::unique_ptr<MCPEPacket> packet, QueuedPacketCallback &&sentCallback) {
    packetQueueMutex.lock();
    packetQueue.push_back({ packet.release(), std::move(sentCallback) });
    packetQueueMutex.unlock();
}

int MCPEPlayer::directPacket(MCPEPacket *packet) {
    RakNet::BitStream bs;
    bs.Write((RakNet::MessageID) packet->id);
    packet->write(bs);
    return this->protocol.getPeer()->Send(&bs, MEDIUM_PRIORITY, packet->reliable ? (packet->needsACK ? RELIABLE_WITH_ACK_RECEIPT : RELIABLE) : UNRELIABLE, 0, address, false);
}

int MCPEPlayer::writePacket(std::unique_ptr<MCPEPacket> packet, bool batch) {
    if (batch) {
        batchPacketCallback(std::move(packet), [](MCPEPlayer *player, MCPEPacket *pk, int pkId) { });
        return 0;
    }

    MCPEPacket* pk = packet.release();
    int ret = directPacket(pk);
    delete pk;
    return ret;
}

bool MCPEPlayer::sendChunk(int x, int z) {
    if (!Player::sendChunk(x, z)) return false;
    Chunk* chunk = this->world.getChunkAt(x, z, true);
    if (chunk == null) return false;

    std::unique_ptr<MCPEFullChunkDataPacket> pk (new MCPEFullChunkDataPacket());
    pk->chunk = chunk;
    pk->needsACK = true;
    batchPacketCallback(std::move(pk), [](MCPEPlayer* player, MCPEPacket* pk, int pkId) {
        MCPEFullChunkDataPacket* fpk = (MCPEFullChunkDataPacket*) pk;
        player->raknetChunkQueue[pkId].push_back(ChunkPos(fpk->chunk->pos.x, fpk->chunk->pos.z));
    });
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

    std::unique_ptr<MCPERespawnPacket> pk (new MCPERespawnPacket());
    pk->x = x;
    pk->y = y;
    pk->z = z;
    writePacket(std::move(pk));

    sendWorldTime(getWorld().getTime(), getWorld().isTimeStopped());

    std::unique_ptr<MCPEPlayStatusPacket> pk2 (new MCPEPlayStatusPacket());
    pk2->status = MCPEPlayStatusPacket::Status::PLAYER_SPAWN;
    writePacket(std::move(pk2));

    sendInventory();
}

void MCPEPlayer::receivedACK(int packetId) {
    if (raknetChunkQueue.count(packetId) > 0) {
        for (ChunkPos pos : raknetChunkQueue.at(packetId))
            MCPEPlayer::receivedChunk(pos.x, pos.z);
    }
}


void MCPEPlayer::close(std::string reason, bool sendToPlayer) {
    if (sendToPlayer) {
        std::unique_ptr<MCPEDisconnectPacket> pk (new MCPEDisconnectPacket());
        pk->message = reason.c_str();
        writePacket(std::move(pk), false);
    }
    Player::close(reason, sendToPlayer);
}

void MCPEPlayer::sendMessage(std::string text) {
    std::unique_ptr<MCPETextPacket> pk (new MCPETextPacket());
    pk->type = MCPETextPacket::MessageType::RAW;
    pk->message = text.c_str();
    writePacket(std::move(pk));
}

void MCPEPlayer::sendPosition(float x, float y, float z) {
    std::unique_ptr<MCPEMovePlayerPacket> pk (new MCPEMovePlayerPacket());
    pk->eid = 0;
    pk->x = x;
    pk->y = y;
    pk->z = z;
    pk->yaw = pk->headYaw = pk->pitch = 0;
    pk->mode = MCPEMovePlayerPacket::Mode::RESET;
    pk->onGround = false;
    writePacket(std::move(pk));
}

void MCPEPlayer::spawnEntity(Entity *entity) {
    Player::spawnEntity(entity);

    if (closed)
        return;
    if (entity->getTypeName() == Player::TYPE_NAME) {
        Logger::main->trace("MCPE/Player", "Spawning player: %s", ((Player*) entity)->getName().c_str());
        unsigned char skin[64 * 32 * 4];

        UUID uuid = {1, entity->getId()};

        std::unique_ptr<MCPEPlayerListPacket> lpk (new MCPEPlayerListPacket());
        lpk->type = MCPEPlayerListPacket::Type::ADD;
        lpk->addEntries.push_back({uuid, entity->getId(), ((Player*) entity)->getName().c_str(), false, &skin[0]});
        writePacket(std::move(lpk));

        std::unique_ptr<MCPEAddPlayerPacket> pk (new MCPEAddPlayerPacket());
        pk->uuid = uuid;
        pk->eid = entity->getId();
        pk->username = ((Player*) entity)->getName().c_str();
        Vector3D v = entity->getPos();
        pk->x = v.x;
        pk->y = v.y;
        pk->z = v.z;
        pk->yaw = pk->headYaw = 0;
        pk->pitch = 0;
        writePacket(std::move(pk));

        updateEntityPos(entity);
        return;
    }
}
void MCPEPlayer::despawnEntity(Entity *entity) {
    Player::despawnEntity(entity);
    if (closed)
        return;
    if (entity->getTypeName() == Player::TYPE_NAME) {
        Logger::main->trace("MCPE/Player", "Despawning player: %s", ((Player*) entity)->getName().c_str());

        UUID uuid = {1, entity->getId()};

        std::unique_ptr<MCPERemovePlayerPacket> pk (new MCPERemovePlayerPacket());
        pk->uuid = uuid;
        pk->eid = entity->getId();
        writePacket(std::move(pk));

        std::unique_ptr<MCPEPlayerListPacket> lpk (new MCPEPlayerListPacket());
        lpk->type = MCPEPlayerListPacket::Type::REMOVE;
        lpk->removeEntries.push_back({uuid});
        writePacket(std::move(lpk));
        return;
    }
}

void MCPEPlayer::updateEntityPos(Entity *entity) {
    std::unique_ptr<MCPEMoveEntityPacket> pk (new MCPEMoveEntityPacket()); // TODO: Batch
    Vector3D pos = entity->getPos();
    pk->entries.push_back({entity->getId(), pos.x, pos.y, pos.z, 0, 0, 0});
    writePacket(std::move(pk));
}

void MCPEPlayer::sendInventorySlot(int slotId) {
    std::unique_ptr<MCPEContainerSetSlotPacket> pk (new MCPEContainerSetSlotPacket());
    pk->window = 0;
    pk->slot = (short) slotId;
    pk->item = inventory.getItem(slotId);
    writePacket(std::move(pk));
}

void MCPEPlayer::sendInventory() {
    std::unique_ptr<MCPEContainerSetContentPacket> pk (new MCPEContainerSetContentPacket());
    pk->window = 0;
    inventory.mutex.lock();
    pk->items = inventory.items;
    inventory.mutex.unlock();
    generalMutex.lock();
    pk->hotbar.resize(9);
    for (int i = 0; i < 9; i++)
        pk->hotbar[i] = hotbarSlots[i] + 9;
    generalMutex.unlock();
    writePacket(std::move(pk));
    sendHeldItem();
}

void MCPEPlayer::sendHeldItem() {
    std::unique_ptr<MCPEMobEquipmentPacket> pk (new MCPEMobEquipmentPacket());
    pk->eid = 0;
    pk->item = inventory.getHeldItem();
    generalMutex.lock();
    pk->slot = inventory.getHeldSlot();
    pk->hotbarSlot = hotbarSlot;
    generalMutex.unlock();
    writePacket(std::move(pk));
}

void MCPEPlayer::linkHeldItem(int hotbarSlot, int inventorySlot) {
    generalMutex.lock();
    this->hotbarSlot = hotbarSlot;
    inventory.setHeldSlot(inventorySlot);
    hotbarSlots[hotbarSlot] = inventorySlot;
    generalMutex.unlock();
}

void MCPEPlayer::sendBlockUpdate(BlockPos bpos) {
    std::unique_ptr<MCPEUpdateBlockPacket> pk (new MCPEUpdateBlockPacket());
    pk->add(world, bpos.x, bpos.y, bpos.z, MCPEUpdateBlockPacket::FLAG_ALL);
    writePacket(std::move(pk));
}

void MCPEPlayer::sendWorldTime(int time, bool stopped) {
    std::unique_ptr<MCPESetTimePacket> pk (new MCPESetTimePacket());
    pk->time = time;
    pk->started = true;
    writePacket(std::move(pk));

    std::unique_ptr<MCPESetTimePacket> pk2 (new MCPESetTimePacket());
    pk2->time = time;
    pk2->started = !stopped;
    writePacket(std::move(pk2));
}