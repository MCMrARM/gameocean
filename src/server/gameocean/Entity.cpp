#include "Entity.h"

#include <gameocean/common.h>
#include "world/World.h"
#include "world/Chunk.h"
#include "Player.h"

EntityId Entity::currentId = 1;

void Entity::close() {
    generalMutex.lock();
    if (closed) {
        generalMutex.unlock();
        return;
    }

    closed = true;

    despawnFromAll();
    if (chunk != null) {
        chunk->removeEntity(this);
    }
    generalMutex.unlock();
}

void Entity::setPos(float x, float y, float z) {
    generalMutex.lock();
    this->x = x;
    this->y = y;
    this->z = z;

    int newChunkX = ((int) x) >> 4;
    int newChunkZ = ((int) z) >> 4;
    if (chunk == null) {
        chunk = world.getChunkAt(newChunkX, newChunkZ);
        chunk->addEntity(this);
        updateViewers();
    } else if (newChunkX != chunk->pos.x || newChunkZ != chunk->pos.z) {
        chunk->removeEntity(this);
        chunk = world.getChunkAt(newChunkX, newChunkZ, true);
        chunk->addEntity(this);
        updateViewers();
    }
    for (Player* p : spawnedTo) {
        p->updateEntityPos(this);
    }
    generalMutex.unlock();
}

void Entity::updateViewers() {
    std::set<Player*> despawnFromPlayers (spawnedTo);
    for (Player* p : chunk->usedBy) {
        if (despawnFromPlayers.count(p) > 0) {
            despawnFromPlayers.erase(p);
        } else {
            spawnTo(p);
        }
    }
    for (Player* p : despawnFromPlayers) {
        despawnFrom(p);
    }
}

void Entity::spawnTo(Player *player) {
    generalMutex.lock();
    if (spawnedTo.count(player) > 0 || player == this) {
        generalMutex.unlock();
        return;
    }
    spawnedTo.insert(player);
    player->spawnEntity(this);
    generalMutex.unlock();
}

void Entity::despawnFrom(Player *player) {
    generalMutex.lock();
    if (spawnedTo.count(player) <= 0) {
        generalMutex.unlock();
        return;
    }
    spawnedTo.erase(player);
    player->despawnEntity(this);
    generalMutex.unlock();
}

void Entity::spawnToAll() {
    generalMutex.lock();
    for (Player* p : chunk->usedBy) {
        spawnTo(p);
    }
    generalMutex.unlock();
}

void Entity::despawnFromAll() {
    generalMutex.lock();
    for (Player* p : spawnedTo) {
        p->despawnEntity(this);
    }
    spawnedTo.empty();
    generalMutex.unlock();
}