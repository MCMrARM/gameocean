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
#include "plugin/event/player/PlayerBlockDestroyEvent.h"
#include <gameocean/entity/ItemEntity.h>
#include "permission/Permission.h"

const char *Player::TYPE_NAME = "Player";

Player::Player(Server &server) : Entity(*server.mainWorld), server(server), shouldUpdateChunkQueue(false), spawned(false), teleporting(false), inventory(*this, 36), transaction(*this) {
    maxHp = hp = 20.f;
    sizeX = 0.6f;
    sizeY = 1.8f;
    headY = 1.62f;
    collisionInBlockAcceptance = 0.2f;
    world->addPlayer(this);

    grantPermissions(Permission::getPlayerPermissions(), false);
};

void Player::close(std::string reason, bool sendToPlayer) {
    if (closed)
        return;

    {
        PlayerQuitEvent event(*this, reason);
        Event::broadcast(event);
    }

    world->removePlayer(this);

    Entity::close();
    for (auto entry : sentChunks) {
        entry.second->setUsedBy(this, false);
    }
    for (auto entry : receivedChunks) {
        entry.second->setUsedBy(this, false);
    }
}

void Player::addTransaction(Inventory &inventory, InventoryTransaction::InventoryKind kind, int slot, ItemInstance to) {
    if (transaction.elements.size() <= 0)
        transactionStart = Time::now();
    else if (Time::now() - transactionStart > 500) {
        transaction.revert();
        transaction.reset();
        transactionStart = Time::now();
    }
    transaction.elements.push_back({ inventory, kind, slot, (kind == InventoryTransaction::InventoryKind::ARMOR ? ((InventoryWithArmor&) inventory).getArmorSlot(slot) : inventory.getItem(slot)), to });
    if (transaction.isFinished()) {
        if (transaction.isValid())
            transaction.execute();
        else
            transaction.revert();
        transaction.reset();
    }
}

void Player::broadcastArmorChange() {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    for (Player *viewer : spawnedTo) {
        viewer->sendPlayerArmor(this);
    }
}

void Player::broadcastHeldItem() {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    for (Player *viewer : spawnedTo) {
        viewer->sendPlayerHeldItem(this);
    }
}

bool Player::sendChunk(int x, int z) {
    ChunkPos pos (x, z);
    ChunkPtr chunk = world->getChunkAt(pos, true);
    if (!chunk)
        return false;
    chunkArrayMutex.lock();
    if (!chunk->ready) {
        toSendChunks[pos] = chunk;
        chunkArrayMutex.unlock();
        return false;
    }
    toSendChunks.erase(pos);
    sentChunks[pos] = chunk;
    chunkArrayMutex.unlock();
    return true;
}

void Player::receivedChunk(int x, int z) {
    chunkArrayMutex.lock();
    ChunkPos pos (x, z);
    if (sentChunks.count(pos) > 0) {
        ChunkPtr chunk = sentChunks[pos];
        receivedChunks[pos] = chunk;
        chunk->setUsedBy(this, true);
    }
    chunkArrayMutex.unlock();
}

void Player::setSpawned() {
    if (spawned)
        return;

    spawned = true;
    chunkArrayMutex.lock();
    for (auto const &entry : sentChunks) {
        ChunkPtr c = entry.second;
        std::lock_guard<std::recursive_mutex> guard (c->entityMutex);
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

void Player::setWorld(World &world, float x, float y, float z) {
    this->world->removePlayer(this);
    Entity::setWorld(world, x, y, z);
    chunkArrayMutex.lock();
    for (auto &entry : toSendChunks)
        entry.second->setUsedBy(this, false);
    toSendChunks.clear();
    for (auto &entry : sentChunks)
        entry.second->setUsedBy(this, false);
    sentChunks.clear();
    for (auto &entry : receivedChunks)
        entry.second->setUsedBy(this, false);
    receivedChunks.clear();
    sendChunksQueue.clear();
    shouldUpdateChunkQueue = true;
    chunkArrayMutex.unlock();
    world.addPlayer(this);
}

void Player::setPos(float x, float y, float z) {
    ChunkPtr oldChunk = chunk;
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

    Vector3D prevPos = getPos();
    Vector3D pos = prevPos;
    pos.add(checkCollisions(x - pos.x, y - pos.y, z - pos.z));

    PlayerMoveEvent event (*this, {pos.x, pos.y, pos.z});
    Event::broadcast(event);
    if (event.isCancelled())
        return false;
    if (teleporting)
        return true; // plugins might have teleported the player in the event handler
    generalMutex.lock();
    bool wasOnGround = onGround;
    pos = event.getPos();
    setPos(pos.x, pos.y, pos.z);

    float multiplier = isInFluid() ? 0.015f : (isSprinting ? 0.1f : 0.01f);
    addFoodExhaustion(multiplier * std::sqrt(std::pow(pos.x - prevPos.x, 2) + std::pow(pos.z - prevPos.z, 2)));
    if (wasOnGround && pos.y - prevPos.y > 0)
        addFoodExhaustion(isSprinting ? 0.8f : 0.2f);
    generalMutex.unlock();

    return (std::fabs(x - event.getPos().x) < 0.01 && std::fabs(y - event.getPos().y) < 0.01 && std::fabs(z - event.getPos().z) < 0.01);
}

bool Player::isInFluid() {
    std::lock_guard<std::recursive_mutex> lock (generalMutex);
    if (y < 0.f)
        return false;
    BlockVariant *v = world->getBlock((int) (x + 0.5f), (int) (y + 0.5f), (int) (z + 0.5f)).getBlockVariant();
    return (v != nullptr && v->fluid);
}

bool Player::isUnderFluid() {
    std::lock_guard<std::recursive_mutex> lock (generalMutex);
    if (y < 0.f)
        return false;
    BlockVariant *v = world->getBlock((int) (x + 0.5f), (int) (aabb.maxY), (int) (z + 0.5f)).getBlockVariant();
    return (v != nullptr && v->fluid);
}

void Player::sendQueuedChunks() {
    if (!chunk)
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
    if (!chunk)
        return;

    if (!shouldUpdateChunkQueue) {
        return;
    }
    shouldUpdateChunkQueue = false;

    chunkArrayMutex.lock();
    sendChunksQueue.clear();

    std::unordered_map<ChunkPos, ChunkPtr> remToSendChunks = toSendChunks;
    std::unordered_map<ChunkPos, ChunkPtr> remSentChunks = sentChunks;
    std::unordered_map<ChunkPos, ChunkPtr> remReceivedChunks = receivedChunks;

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
        remToSendChunks.erase(currentPos);

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
    for (auto entry : remToSendChunks) {
        toSendChunks.erase(entry.first);
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
    if (miningBlock == nullptr)
        return;
    sendBlockUpdate(miningBlockPos);
    miningBlockPos = {};
    miningBlock = nullptr;
    miningStarted = 0;
}

void Player::finishedMining() {
    if (miningBlock != world->getBlock(miningBlockPos).getBlockVariant()) {
        cancelMining();
        return;
    }
    PlayerBlockDestroyEvent event (*this, getWorld(), miningBlock, miningBlockPos);
    Event::broadcast(event);
    if (event.isCancelled())
        return;
    {
        ItemInstance itm = inventory.getItem(inventory.heldSlot);
        if (itm.getItem() != nullptr) {
            itm.damageItem(itm.getItem()->itemDamageBlockBreak);
            inventory.setItem(inventory.heldSlot, itm);
        }
    }
    world->setBlock(miningBlockPos, 0, 0);
    miningBlock->dropItems(*world, miningBlockPos, inventory.getHeldItem().getItem());
    addFoodExhaustion(0.025f);
    cancelMining();
}

int Player::getRemainingMiningTime() {
    if (miningBlock == nullptr)
        return -1;
    return (int) (miningTime - (Time::now() - miningStarted));
}

int Player::calculateMiningTime() {
    if (miningBlock == nullptr)
        return -1;

    float r = miningBlock->hardness * 1500.0f;
    bool hasTool = false;
    ItemVariant *held = inventory.getHeldItem().getItem();
    if (held != nullptr && miningBlock->blockGroup != nullptr) {
        if (held->toolAffects.count(miningBlock->blockGroup) > 0) {
            hasTool = true;
            r /= held->toolBreakMultiplier;
        }
    }

    if (miningBlock->needsTool && !hasTool) {
        r *= 3.3f;
    } else {
        if (!onGround)
            r *= 5.f;
        if (isUnderFluid())
            r *= 5.f;
    }
    return (int) r;
}

void Player::processMessage(std::string text) {
    if (text[0] == '/') { // command
        std::vector<std::string> v = StringUtils::split(text.substr(1), " ");

        if (v.size() <= 0)
            return;

        std::string commandName = v[0];

        Command *c = Command::getCommand(commandName);
        if (c == nullptr) {
            sendMessage("Command not found.");
            return;
        }
        Permission *p = c->getRequiredPermission();
        if (p != nullptr && !hasPermission(p)) {
            sendMessage("You don't have enough permissions!");
            return;
        }
        c->process(*this, v);
    } else if (spawned) {
        ChatEvent event (*this, text, "<%s> %s");
        Event::broadcast(event);
        server.broadcastMessage(StringUtils::sprintf(event.getFormat(), getName().c_str(), event.getMessage().c_str()));
    }
}

void Player::attack(Entity &entity) {
    float damage = 1.f;
    if (!inventory.getHeldItem().isEmpty())
        damage = inventory.getHeldItem().getItem()->attackDamage;

    EntityDamageEvent event (entity, damage, EntityDamageEvent::DamageSource::ENTITY, this, 0.3f);

    PlayerAttackEvent attackEvent (*this, event);
    Event::broadcast(attackEvent);

    if (attackEvent.isCancelled())
        return;

    {
        ItemInstance itm = inventory.getItem(inventory.heldSlot);
        if (itm.getItem() != nullptr) {
            itm.damageItem(itm.getItem()->itemDamageAttack);
            inventory.setItem(inventory.heldSlot, itm);
        }
    }
    addFoodExhaustion(0.3f);
    entity.damage(event);
}

void Player::damage(EntityDamageEvent &event) {
    event.setDamage(std::round(event.getDamage() * getArmorReductionMultiplier()));

    PlayerDamageEvent damageEvent (*this, event);
    Event::broadcast(damageEvent);
    if (damageEvent.isCancelled())
        return;
    Entity::damage(event);
    for (Player* player : spawnedTo) {
        player->sendHurtAnimation(this);
    }
    sendHurtAnimation(this);
    addFoodExhaustion(0.3f);
}

float Player::getArmorReductionMultiplier() {
    float m = 1.f;
    for (int i = 0; i < 4; i++) {
        ItemInstance itm = inventory.getArmorSlot(i);
        if (!itm.isEmpty())
            m -= itm.getItem()->damageReduction;
    }
    return m;
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
    chunk = ChunkPtr();

    sendDeathStatus();
    chunkArrayMutex.lock();
    sendChunksQueue.clear();
    for (auto entry : sentChunks) {
        entry.second->setUsedBy(this, false);
    }
    chunkArrayMutex.unlock();
    despawnFromAll();
}

void Player::setOperator(bool op) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    this->isOp = op;
    if (op)
        grantPermissions(Permission::getOperatorPermissions(), false);
    else
        removePermissions(Permission::getOperatorPermissions(), false);
}

bool Player::hasPermission(Permission *perm) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    return (permissions.count(perm) > 0);
}

void Player::grantPermissions(std::set<Permission *> perms, bool children) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    for (Permission *perm : perms) {
        permissions.insert(perm);
        if (children) {
            for (Permission *child : perm->children) {
                perms.insert(child);
            }
        }
    }
}

void Player::removePermissions(std::set<Permission *> perms, bool children) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    for (Permission *perm : perms) {
        permissions.erase(perm);
        if (children) {
            for (Permission *child : perm->children) {
                perms.insert(child);
            }
        }
    }
}

void* Player::getPluginData(Plugin *plugin) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    if (pluginData.count(plugin) > 0)
        return pluginData.at(plugin);
    return nullptr;
}

void Player::setPluginData(Plugin *plugin, void *data) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    pluginData[plugin] = data;
}

void Player::tickPhysics() {
    world->getNearbyEntities(getAABB().expand(1.f, 0.5f, 1.f), [this](std::shared_ptr<Entity> ent) {
        if (ent->getTypeName() == ItemEntity::TYPE_NAME) {
            inventory.addItem(((ItemEntity *) &*ent)->getItem());
            ent->kill();
        }
    });
}

void Player::update() {
    tickHunger();
}

void Player::setHunger(float points) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    hunger = points;
    sendHunger(hunger);
}

float Player::getHunger() {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    return hunger;
}

void Player::restoreHunger(float hunger, float saturation) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    this->hunger = std::max(std::min(this->hunger + hunger, 20.f), 0.f);
    foodSaturation = std::max(std::min(foodSaturation + saturation, this->hunger), 0.f);
    sendHunger(this->hunger);
}

void Player::tickHunger() {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    if (hungerDisabled)
        return;
    long long now = Time::now();
    if (hunger <= 0.f && (now - foodLastHpChange) >= 4000) {
        EntityDamageEvent event (*this, 1.f, EntityDamageEvent::DamageSource::HUNGER);
        damage(event);
        foodLastHpChange = Time::now();
    } else if (hp < 20.f) {
        if (hunger >= 20.f && foodSaturation > 0.f && (now - foodLastHpChange) >= 500) {
            setHealth(hp + 1.f);
            foodExhaustion += 4.f;
            foodLastHpChange = Time::now();
        } else if (hunger >= 18.f && (now - foodLastHpChange) >= 4000) {
            setHealth(hp + 1.f);
            foodExhaustion += 3.f;
            foodLastHpChange = Time::now();
        }
    } else {
        foodLastHpChange = Time::now();
    }

    if (foodExhaustion > 4.f) {
        foodSaturation = std::max(0.f, foodSaturation - 1.f);
        foodExhaustion -= 4.f;
        if (foodSaturation <= 0.f) {
            hunger -= 1.f;
            sendHunger(hunger);
        }
    }
    generalMutex.unlock();
}

void Player::addFoodExhaustion(float amount) {
    generalMutex.lock();
    if (!hungerDisabled)
        foodExhaustion += amount;
    generalMutex.unlock();
}