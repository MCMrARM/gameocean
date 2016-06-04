#include "MCPEPlayer.h"

#include <iostream>
#include <string>
#include "MCPEPacket.h"
#include "MCPEPacketWrapper.h"
#include "MCPEConnection.h"
#include <gameocean/world/World.h>
#include <gameocean/world/Chunk.h>
#include <gameocean/world/tile/Container.h>
#include <gameocean/entity/ItemEntity.h>
#include <gameocean/entity/Snowball.h>

void MCPEPlayer::batchPacketCallback(std::unique_ptr<MCPEPacket> packet, QueuedPacketCallback &&sentCallback) {
    packetQueueMutex.lock();
    packetQueue.push_back({ std::move(packet), std::move(sentCallback) });
    packetQueueMutex.unlock();
}

int MCPEPlayer::directPacket(MCPEPacket *packet) {
    MCPESendPacketWrapper wrapper (*packet);
    return connection.send(wrapper, packet->reliable ? (packet->needsACK ? RakNetReliability::RELIABLE_ACK_RECEIPT : RakNetReliability::RELIABLE) : RakNetReliability::UNRELIABLE);
}

int MCPEPlayer::writePacket(std::unique_ptr<MCPEPacket> packet, bool batch) {
    if (batch) {
        batchPacketCallback(std::move(packet), [](MCPEPacket *pk, int pkId) { });
        return 0;
    }

    return directPacket(&*packet);
}

bool MCPEPlayer::sendChunk(int x, int z) {
    if (!Player::sendChunk(x, z)) return false;
    ChunkPtr chunk = this->world->getChunkAt(x, z, true);
    if (!chunk) return false;

    std::unique_ptr<MCPEFullChunkDataPacket> pk (new MCPEFullChunkDataPacket());
    pk->chunk = chunk;
    pk->needsACK = true;
    batchPacketCallback(std::move(pk), [this](MCPEPacket* pk, int pkId) {
        MCPEFullChunkDataPacket* fpk = (MCPEFullChunkDataPacket*) pk;
        chunkArrayMutex.lock();
        raknetChunkQueue[pkId].push_back(ChunkPos(fpk->chunk->pos.x, fpk->chunk->pos.z));
        chunkArrayMutex.unlock();
        receivedACK(pkId); //TODO:This is temporary workaround
    });
    return true;
}

void MCPEPlayer::receivedChunk(int x, int z) {
    Player::receivedChunk(x, z);

    ChunkPtr chunk = world->getChunkAt({x, z}, false);
    if (chunk) {
        std::unique_lock<std::mutex> lock (chunk->tilesMutex);
        for (std::shared_ptr<Tile> tile : chunk->tiles) {
            std::unique_ptr<MCPETileEntityDataPacket> pk (new MCPETileEntityDataPacket());
            pk->tile = tile;
            writePacket(std::move(pk));
        }
    }

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
    chunkArrayMutex.lock();
    if (raknetChunkQueue.count(packetId) > 0) {
        for (ChunkPos pos : raknetChunkQueue.at(packetId))
            MCPEPlayer::receivedChunk(pos.x, pos.z);
    }
    chunkArrayMutex.unlock();
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
    pk->y = y + getHeadY();
    pk->z = z;
    pk->yaw = pk->headYaw = yaw;
    pk->pitch = pitch;
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
        UUID uuid = {1, entity->getId()};

        std::unique_ptr<MCPEPlayerListPacket> lpk (new MCPEPlayerListPacket());
        lpk->type = MCPEPlayerListPacket::Type::ADD;
        lpk->addEntries.push_back({uuid, entity->getId(), ((Player*) entity)->getName().c_str(), ((Player*) entity)->skinModel.c_str(), ((Player*) entity)->skin});
        writePacket(std::move(lpk));

        std::unique_ptr<MCPEAddPlayerPacket> pk (new MCPEAddPlayerPacket());
        pk->uuid = uuid;
        pk->eid = entity->getId();
        pk->username = ((Player*) entity)->getName().c_str();
        Vector3D v = entity->getHeadPos();
        Vector2D r = entity->getRot();
        pk->x = v.x;
        pk->y = v.y;
        pk->z = v.z;
        pk->yaw = pk->headYaw = r.x;
        pk->pitch = r.y;
        writePacket(std::move(pk));

        updateEntityPos(entity);
        sendPlayerArmor((Player*) entity);
        sendPlayerHeldItem((Player*) entity);
        return;
    } else if (entity->getTypeName() == ItemEntity::TYPE_NAME) {
        std::unique_ptr<MCPEAddItemEntityPacket> pk (new MCPEAddItemEntityPacket());
        pk->eid = entity->getId();
        pk->item = ((ItemEntity*) entity)->getItem();
        Vector3D v = entity->getHeadPos();
        Vector3D m = entity->getMotion();
        pk->x = v.x;
        pk->y = v.y;
        pk->z = v.z;
        pk->motionX = m.x;
        pk->motionY = m.y;
        pk->motionZ = m.z;
        writePacket(std::move(pk));
        return;
    } else if (entity->getTypeName() == Snowball::TYPE_NAME) {
        std::unique_ptr<MCPEAddEntityPacket> pk (new MCPEAddEntityPacket());
        pk->eid = entity->getId();
        pk->typeId = 81;
        Vector3D v = entity->getHeadPos();
        Vector2D r = entity->getRot();
        Vector3D m = entity->getMotion();
        pk->x = v.x;
        pk->y = v.y;
        pk->z = v.z;
        pk->motionX = m.x;
        pk->motionY = m.y;
        pk->motionZ = m.z;
        pk->yaw = r.x;
        pk->pitch = r.y;
        pk->metadata.setByte(MCPEEntityMetadata::FLAGS, 0);
        pk->metadata.setShort(MCPEEntityMetadata::AIR, 300);
        pk->metadata.setString(MCPEEntityMetadata::NAMETAG, "");
        pk->metadata.setByte(MCPEEntityMetadata::SHOW_NAMETAG, 0);
        pk->metadata.setByte(MCPEEntityMetadata::SILENT, 0);
        pk->metadata.setByte(MCPEEntityMetadata::NO_AI, 0);
        if (((Snowball*) entity)->getThrownBy() != nullptr)
            pk->metadata.setLong(17, ((Snowball*) entity)->getThrownBy()->getId());
        writePacket(std::move(pk));
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
    } else {
        std::unique_ptr<MCPERemoveEntityPacket> pk (new MCPERemoveEntityPacket());
        pk->eid = entity->getId();
        writePacket(std::move(pk));
    }
}

void MCPEPlayer::updateEntityPos(Entity *entity) {
    std::unique_ptr<MCPEMoveEntityPacket> pk (new MCPEMoveEntityPacket()); // TODO: Batch
    Vector3D pos = entity->getHeadPos();
    Vector2D rot = entity->getRot();
    pk->entries.push_back({entity->getId(), pos.x, pos.y, pos.z, rot.x, rot.x, rot.y});
    writePacket(std::move(pk));
}

void MCPEPlayer::sendHurtAnimation(Entity* entity) {
    std::unique_ptr<MCPEEntityEventPacket> pk (new MCPEEntityEventPacket());
    pk->eid = (entity == this ? 0 : entity->getId());
    pk->event = MCPEEntityEventPacket::Event::HURT_ANIMATION;
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

void MCPEPlayer::sendPlayerHeldItem(Player* player) {
    std::unique_ptr<MCPEMobEquipmentPacket> pk (new MCPEMobEquipmentPacket());
    pk->eid = (player == this ? 0 : player->getId());
    pk->item = player->inventory.getHeldItem();
    generalMutex.lock();
    pk->slot = (byte) (player == this ? inventory.getHeldSlot() : 0);
    pk->hotbarSlot = (byte) (player == this ? hotbarSlot : 0);
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

void MCPEPlayer::sendPlayerArmor(Player* player) {
    std::unique_ptr<MCPEMobArmorEquipmentPacket> pk (new MCPEMobArmorEquipmentPacket());
    pk->eid = (player == this ? 0 : player->getId());
    for (int i = 0; i < 4; i++)
        pk->slots[i] = player->inventory.getArmorSlot(i);
    writePacket(std::move(pk));
}

void MCPEPlayer::sendBlockUpdate(BlockPos bpos) {
    std::unique_ptr<MCPEUpdateBlockPacket> pk (new MCPEUpdateBlockPacket());
    pk->add(*world, bpos.x, bpos.y, bpos.z, MCPEUpdateBlockPacket::FLAG_ALL);
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

void MCPEPlayer::sendHealth(float hp) {
    std::unique_ptr<MCPEUpdateAttributesPacket> pk (new MCPEUpdateAttributesPacket());
    pk->eid = 0;
    pk->entries.push_back({0.f, 20.f, hp, MCPEUpdateAttributesPacket::ATTRIBUTE_HEALTH });
    writePacket(std::move(pk));
}

void MCPEPlayer::sendHunger(float hunger) {
    std::unique_ptr<MCPEUpdateAttributesPacket> pk (new MCPEUpdateAttributesPacket());
    pk->eid = 0;
    pk->entries.push_back({0.f, 20.f, hunger, MCPEUpdateAttributesPacket::ATTRIBUTE_HUNGER });
    writePacket(std::move(pk));
}

void MCPEPlayer::sendDeathStatus() {
    sendHealth(0);
    std::unique_ptr<MCPERespawnPacket> pk (new MCPERespawnPacket());
    pk->x = x;
    pk->y = y;
    pk->z = z;
    writePacket(std::move(pk));
}

void MCPEPlayer::openContainer(std::shared_ptr<Container> container) {
    Player::openContainer(container);
    std::unique_ptr<MCPEContainerOpenPacket> pk (new MCPEContainerOpenPacket());
    pk->window = 2;
    pk->type = 0;
    pk->slots = (short) container->getInventory().getNumSlots();
    pk->x = container->getPos().x;
    pk->y = container->getPos().y;
    pk->z = container->getPos().z;
    writePacket(std::move(pk));

    sendContainerContents();
}

void MCPEPlayer::sendContainerContents() {
    if (!openedContainer)
        return;
    std::unique_ptr<MCPEContainerSetContentPacket> pk (new MCPEContainerSetContentPacket());
    pk->window = 2;
    openedContainer->getInventory().mutex.lock();
    pk->items = openedContainer->getInventory().items;
    openedContainer->getInventory().mutex.unlock();
    writePacket(std::move(pk));
}

void MCPEPlayer::closeContainer() {
    Player::closeContainer();
    std::unique_ptr<MCPEContainerClosePacket> pk (new MCPEContainerClosePacket());
    pk->window = 2;
    writePacket(std::move(pk));
}

void MCPEPlayer::setMotion(Vector3D motion) {
    Entity::setMotion(motion);
    std::unique_ptr<MCPESetEntityMotionPacket> pk (new MCPESetEntityMotionPacket());
    pk->entries.push_back({ 0, motion.x, motion.y, motion.z });
    writePacket(std::move(pk));
}