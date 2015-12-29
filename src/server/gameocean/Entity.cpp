#include "Entity.h"

#include <gameocean/common.h>
#include "world/World.h"
#include "world/Chunk.h"
#include "Player.h"
#include "plugin/event/entity/EntityDamageEvent.h"

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

void Entity::setWorld(World& world, float x, float y, float z) {
    generalMutex.lock();
    this->world = &world;
    this->x = x;
    this->y = y;
    this->z = z;

    int newChunkX = ((int) x) >> 4;
    int newChunkZ = ((int) z) >> 4;
    if (chunk == null) {
        chunk = world.getChunkAt(newChunkX, newChunkZ);
        chunk->addEntity(this);
        updateViewers();
    } else {
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

void Entity::setPos(float x, float y, float z) {
    generalMutex.lock();
    this->x = x;
    this->y = y;
    this->z = z;

    int newChunkX = ((int) x) >> 4;
    int newChunkZ = ((int) z) >> 4;
    if (chunk == null) {
        chunk = world->getChunkAt(newChunkX, newChunkZ);
        chunk->addEntity(this);
        updateViewers();
    } else if (newChunkX != chunk->pos.x || newChunkZ != chunk->pos.z) {
        chunk->removeEntity(this);
        chunk = world->getChunkAt(newChunkX, newChunkZ, true);
        chunk->addEntity(this);
        updateViewers();
    }
    for (Player* p : spawnedTo) {
        p->updateEntityPos(this);
    }
    generalMutex.unlock();
}

void Entity::setRot(float yaw, float pitch) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    this->yaw = yaw;
    this->pitch = pitch;
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
    spawnedTo.clear();
    generalMutex.unlock();
}

void Entity::damage(EntityDamageEvent& event) {
    Event::broadcast(event);

    if (event.isCancelled())
        return;

    if (event.getAttacker() != null) {
        Vector3D pos = event.getAttacker()->getPos();
        generalMutex.lock();
        knockBack(pos.x - x, pos.z - z, event.getKnockback());
        generalMutex.unlock();
    }
    setHealth(getHealth() - event.getDamage());
}

void Entity::knockBack(float x, float z, float force) {
    float s = sqrtf(x * x + z * z);
    if (s <= 0)
        return;
    s = 1 / s;
    motion.x = motion.x / 2 + x * s * force;
    motion.y = motion.y / 2 + force;
    motion.z = motion.z / 2 + z * s * force;
    if (motion.y > force)
        motion.y = force;
}

std::vector<Entity*> Entity::getNearbyEntities(float range) {
    Vector3D myPos = getPos();
    World& world = getWorld();
    std::vector<Entity*> ret;
    float minX = myPos.x - range;
    float minY = myPos.y - range;
    float minZ = myPos.z - range;
    float maxX = myPos.x + range;
    float maxY = myPos.y + range;
    float maxZ = myPos.z + range;
    for (int chunkX = (((int) myPos.x - (int) ceil(range)) >> 4); chunkX <= (((int) myPos.x + (int) ceil(range)) >> 4); chunkX++) {
        for (int chunkZ = (((int) myPos.z - (int) ceil(range)) >> 4); chunkZ <= (((int) myPos.z + (int) ceil(range)) >> 4); chunkZ++) {
            if (world.isChunkLoaded(chunkX, chunkZ)) {
                Chunk* chunk = world.getChunkAt(chunkX, chunkZ, false);
                chunk->mutex.lock();
                for (auto const& e : chunk->entities) {
                    Entity* ent = e.second;
                    Vector3D pos = ent->getPos();
                    if (pos.x >= minX && pos.x <= maxX && pos.y >= minY && pos.y <= maxY && pos.z >= minZ && pos.z <= maxZ)
                        ret.push_back(ent);
                }
                chunk->mutex.unlock();
            }
        }
    }
    return ret;
}