#include "Player.h"

#include <iostream>
#include <gameocean/world/World.h>
#include "command/Command.h"
#include <gameocean/utils/StringUtils.h>
#include <gameocean/utils/Time.h>
#include "plugin/event/player/ChatEvent.h"
#include "plugin/event/player/PlayerQuitEvent.h"
#include "plugin/event/player/PlayerJoinEvent.h"
#include "plugin/event/player/PlayerMoveEvent.h"
#include <gameocean/plugin/event/entity/EntityDamageEvent.h>
#include "plugin/event/player/PlayerAttackEvent.h"
#include "plugin/event/player/PlayerDamageEvent.h"
#include "plugin/event/player/PlayerDeathEvent.h"

const char* Player::TYPE_NAME = "Player";

Player::Player(Server& server) : Entity(*server.mainWorld), server(server), shouldUpdateChunkQueue(false), spawned(false), teleporting(false), inventory(*this, 36), transaction(*this) {
    maxHp = hp = 20.f;
    sizeX = 0.6f;
    sizeY = 1.8f;
    headY = 1.62f;
    world->addPlayer(this);
};

void Player::close(std::string reason, bool sendToPlayer) {
    {
        PlayerQuitEvent event(*this, reason);
        Event::broadcast(event);
    }

    if (closed)
        return;

    world->removePlayer(this);

    Entity::close();
    for (auto entry : sentChunks) {
        entry.second->setUsedBy(this, false);
    }
    for (auto entry : receivedChunks) {
        entry.second->setUsedBy(this, false);
    }
}

void Player::addTransaction(Inventory& inventory, int slot, ItemInstance to) {
    if (transaction.elements.size() <= 0)
        transactionStart = Time::now();
    else if (Time::now() - transactionStart > 500) {
        transaction.revert();
        transaction.reset();
        transactionStart = Time::now();
    }
    transaction.elements.push_back({ inventory, slot, inventory.getItem(slot), to });
    if (transaction.isFinished()) {
        if (transaction.isValid())
            transaction.execute();
        else
            transaction.revert();
        transaction.reset();
    }
}

bool Player::sendChunk(int x, int z) {
    ChunkPos pos (x, z);
    Chunk* chunk = world->getChunkAt(pos, true);
    if (chunk == null || !chunk->ready)
        return false;
    chunkArrayMutex.lock();
    sentChunks[pos] = chunk;
    chunkArrayMutex.unlock();
    return true;
}

void Player::receivedChunk(int x, int z) {
    chunkArrayMutex.lock();
    ChunkPos pos (x, z);
    if (sentChunks.count(pos) > 0) {
        receivedChunks[pos] = world->getChunkAt(pos);
    }
    chunkArrayMutex.unlock();

    Chunk* chunk = world->getChunkAt(pos, false);
    if (chunk != null)
        chunk->setUsedBy(this, true);
}

void Player::setSpawned() {
    if (spawned)
        return;

    spawned = true;
    chunkArrayMutex.lock();
    for (auto entry : sentChunks) {
        Chunk* c = entry.second;
        std::lock_guard<std::recursive_mutex> guard (c->mutex);
        for (auto e : c->entities) {
            e.second->spawnTo(this);
        }
    }
    chunkArrayMutex.unlock();

    {
        PlayerJoinEvent event(*this);
        Event::broadcast(event);
    }
}

void Player::respawn() {
    if (spawned)
        return;

    setHealth(20.f);
    spawned = true;
    teleport(*world, x, y, z);
    chunkArrayMutex.lock();
    for (auto entry : sentChunks) {
        entry.second->setUsedBy(this, true);
    }
    chunkArrayMutex.unlock();
}

void Player::setWorld(World& world, float x, float y, float z) {
    Entity::setWorld(world, x, y, z);
    shouldUpdateChunkQueue = true;
}

void Player::setPos(float x, float y, float z) {
    Chunk* oldChunk = chunk;
    Entity::setPos(x, y, z);

    if (oldChunk != chunk) {
        shouldUpdateChunkQueue = true;
    }
}

void Player::teleport(float x, float y, float z) {
    teleporting = true;
    setPos(x, y, z);
}

void Player::teleport(World& world, float x, float y, float z) {
    teleporting = true;
    setWorld(world, x, y, z);
}

bool Player::tryMove(float x, float y, float z) {
    if (!spawned)
        return false;
    if (teleporting)
        return true; // return true so the position won't be reverted

    Vector3D pos = getPos();
    pos.add(checkCollisions(x - pos.x, y - pos.y, z - pos.z));

    PlayerMoveEvent event (*this, {pos.x, pos.y, pos.z});
    Event::broadcast(event);
    if (event.isCancelled())
        return false;
    setPos(event.getPos().x, event.getPos().y, event.getPos().z);

    return (x == event.getPos().x && y == event.getPos().y && z == event.getPos().z);
}

void Player::sendQueuedChunks() {
    if (chunk == null)
        return;

    chunkArrayMutex.lock();
    int sent = 0;
    for (auto it = sendChunksQueue.begin(); it != sendChunksQueue.end(); ) {
        if (sent > server.sendChunksCount) {
            chunkArrayMutex.unlock();
            return;
        }
        ChunkPos pos = *it;
        if (sendChunk(pos.x, pos.z)) {
            it = sendChunksQueue.erase(it);
            sent++;
        } else {
            it++;
        }
    }
    chunkArrayMutex.unlock();

    updateTeleportState();
}

void Player::updateChunkQueue() {
    if (chunk == null)
        return;

    if (!shouldUpdateChunkQueue) {
        return;
    }
    shouldUpdateChunkQueue = false;

    chunkArrayMutex.lock();
    sendChunksQueue.clear();

    std::unordered_map<ChunkPos, Chunk*> remSentChunks = sentChunks;
    std::unordered_map<ChunkPos, Chunk*> remReceivedChunks = receivedChunks;

    int dir = 0;
    int len = 0;
    int x = 0;
    int z = 0;
    for (int i = 0; i < viewChunks; i++) {
        ChunkPos currentPos (chunk->pos.x + x, chunk->pos.z + z);
        if (sentChunks.count(currentPos) <= 0 && receivedChunks.count(currentPos) <= 0) {
            sendChunksQueue.push_back(currentPos);
        }
        remSentChunks.erase(currentPos);
        remReceivedChunks.erase(currentPos);

        if (dir == 0) {
            x++;
            if (x >= len) dir++;
        } else if (dir == 1) {
            z++;
            if (z >= len) dir++;
        } else if (dir == 2) {
            x--;
            if (-x >= len) dir++;
        } else if (dir == 3) {
            z--;
            if (-z >= len) {
                dir = 0;
                len++;
            }
        }
    }

    for (auto entry : remSentChunks) {
        sentChunks.erase(entry.first);
        entry.second->setUsedBy(this, false);
    }
    for (auto entry : remReceivedChunks) {
        receivedChunks.erase(entry.first);
        entry.second->setUsedBy(this, false);
    }

    chunkArrayMutex.unlock();
}

void Player::updateTeleportState() {
    if (!teleporting) return;

    chunkArrayMutex.lock();
    generalMutex.lock();
    int posX = chunk->pos.x;
    int posZ = chunk->pos.z;
    generalMutex.unlock();
    for (int x = posX - 1; x <= posX + 1; x++) {
        for (int z = posZ - 1; z <= posZ + 1; z++) {
            ChunkPos pos (x, z);
            if (receivedChunks.count(pos) <= 0) {
                chunkArrayMutex.unlock();
                return;
            }
        }
    }
    teleporting = false;
    chunkArrayMutex.unlock();

    generalMutex.lock();
    sendPosition(x, y, z);
    generalMutex.unlock();
}

void Player::startMining(BlockPos pos) {
    miningBlockPos = pos;
    miningBlock = world->getBlock(pos).getBlockVariant();
    miningStarted = Time::now();
    miningTime = calculateMiningTime();
    server.playerBlockDestroyThread.notifyChange();
}

void Player::cancelMining() {
    miningBlockPos = {};
    miningBlock = null;
    miningStarted = 0;
}

void Player::finishedMining() {
    if (miningBlock != world->getBlock(miningBlockPos).getBlockVariant()) {
        cancelMining();
        return;
    }
    world->dropItem({ miningBlockPos.x + 0.5f, miningBlockPos.y + 0.5f, miningBlockPos.z + 0.5f }, ItemInstance (miningBlock, 1, miningBlock->getVariantDataId()));
    world->setBlock(miningBlockPos, 0, 0);
    cancelMining();
}

int Player::getRemainingMiningTime() {
    if (miningBlock == null)
        return -1;
    return miningTime - (Time::now() - miningStarted);
}

int Player::calculateMiningTime() {
    if (miningBlock == null)
        return -1;

    float r = miningBlock->hardness * 1500.0f;
    bool hasTool = false;
    ItemVariant* held = inventory.getHeldItem().getItem();
    if (held != null && miningBlock->blockGroup != null) {
        if (held->toolAffects.count(miningBlock->blockGroup) > 0) {
            hasTool = true;
            r /= held->toolBreakMultiplier;
        }
    }

    if (miningBlock->needsTool && !hasTool) {
        r *= 3.3f;
    }
    // TODO: On ground
    return (int) r;
}

void Player::processMessage(std::string text) {
    if (text[0] == '/') { // command
        std::vector<std::string> v = StringUtils::split(text.substr(1), " ");

        if (v.size() <= 0)
            return;

        std::string commandName = v[0];

        Command* c = Command::getCommand(commandName);
        if (c == null) {
            sendMessage("Command not found.");
            return;
        }
        c->process(*this, v);
    } else if (spawned) {
        ChatEvent event (*this, text, "<%s> %s");
        Event::broadcast(event);
        server.broadcastMessage(StringUtils::sprintf(event.getFormat(), getName().c_str(), event.getMessage().c_str()));
    }
}

void Player::attack(Entity& entity) {
    EntityDamageEvent event (entity, 1.f, EntityDamageEvent::DamageSource::ENTITY, this, 0.3f);

    PlayerAttackEvent attackEvent (*this, event);
    Event::broadcast(attackEvent);

    if (attackEvent.isCancelled())
        return;

    entity.damage(event);
}

void Player::damage(EntityDamageEvent& event) {
    PlayerDamageEvent damageEvent (*this, event);
    Event::broadcast(damageEvent);
    if (damageEvent.isCancelled())
        return;
    Entity::damage(event);
    for (Player* player : spawnedTo) {
        player->sendHurtAnimation(this);
    }
    sendHurtAnimation(this);
}

void Player::setHealth(float hp) {
    if (hp > 0) {
        sendHealth(hp);
    }
    Entity::setHealth(hp);
}

void Player::kill() {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    PlayerDeathEvent event (*this, world, getPos());
    Event::broadcast(event);
    if (event.isCancelled())
        return;

    spawned = false;
    x = event.getRespawnPos().x;
    y = event.getRespawnPos().y;
    z = event.getRespawnPos().z;
    world = event.getWorld();
    chunk->removeEntity(this);
    chunk = null;

    sendDeathStatus();
    chunkArrayMutex.lock();
    sendChunksQueue.clear();
    for (auto entry : sentChunks) {
        entry.second->setUsedBy(this, false);
    }
    chunkArrayMutex.unlock();
    despawnFromAll();
}

void* Player::getPluginData(Plugin* plugin) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    if (pluginData.count(plugin) > 0)
        return pluginData.at(plugin);
    return null;
}

void Player::setPluginData(Plugin* plugin, void* data) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    pluginData[plugin] = data;
}