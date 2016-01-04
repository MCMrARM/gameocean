#include "Entity.h"

#include <gameocean/common.h>
#include "../world/World.h"
#include "../world/Chunk.h"

#ifdef SERVER
#include <gameocean/Player.h>
#endif
#include "../plugin/event/entity/EntityDamageEvent.h"

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
#ifdef SERVER
    for (Player* p : spawnedTo) {
        p->updateEntityPos(this);
    }
#endif
    generalMutex.unlock();
}

void Entity::setPos(float x, float y, float z) {
    generalMutex.lock();
    this->x = x;
    this->y = y;
    this->z = z;
    this->aabb = { x - sizeX / 2, y, z - sizeX / 2, x + sizeX / 2, y + sizeY, z + sizeX / 2 };

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
#ifdef SERVER
    for (Player* p : spawnedTo) {
        p->updateEntityPos(this);
    }
#endif
    generalMutex.unlock();
}

Vector3D Entity::checkCollisions(float x, float y, float z) {
    generalMutex.lock();
    AABB aabb = this->aabb;
    generalMutex.unlock();

    AABB expanded = aabb;
    expanded.add(0, y, 0);
    if (y > 0) {
        world->getBlockBoxes(expanded, [&y, aabb](AABB const& baabb) {
            if (baabb.minX > aabb.maxX || baabb.maxX < aabb.minX ||
                    baabb.minZ > aabb.maxZ || baabb.maxZ < aabb.minZ)
                return;
            if (aabb.maxY <= baabb.minY) {
                float y2 = baabb.minY - aabb.maxY;
                if (y2 < y)
                    y = y2;
            }
        });
    } else if (y < 0) {
        world->getBlockBoxes(expanded, [&y, aabb](AABB const& baabb) {
            if (baabb.minX > aabb.maxX || baabb.maxX < aabb.minX ||
                baabb.minZ > aabb.maxZ || baabb.maxZ < aabb.minZ)
                return;
            if (aabb.minY >= baabb.maxY) {
                float y2 = baabb.maxY - aabb.minY;
                if (y2 > y)
                    y = y2;
            }
        });
    }
    aabb.translate(0, y, 0);
    expanded = aabb;
    expanded.add(x, 0, 0);
    if (x > 0) {
        world->getBlockBoxes(expanded, [&x, aabb](AABB const& baabb) {
            if (baabb.minY > aabb.maxY || baabb.maxY < aabb.minY ||
                baabb.minZ > aabb.maxZ || baabb.maxZ < aabb.minZ)
                return;
            if (aabb.maxX < baabb.minX) {
                float x2 = baabb.minX - aabb.maxX;
                if (x2 < x)
                    x = x2;
            }
        });
    } else if (x < 0) {
        world->getBlockBoxes(expanded, [&x, aabb](AABB const& baabb) {
            if (baabb.minY > aabb.maxY || baabb.maxY < aabb.minY ||
                baabb.minZ > aabb.maxZ || baabb.maxZ < aabb.minZ)
                return;
            if (aabb.minX > baabb.maxX) {
                float x2 = baabb.maxX - aabb.minX;
                if (x2 > x)
                    x = x2;
            }
        });
    }
    aabb.translate(x, 0, 0);
    expanded = aabb;
    expanded.add(0, 0, z);
    if (z > 0) {
        world->getBlockBoxes(expanded, [&z, aabb](AABB const& baabb) {
            if (baabb.minY > aabb.maxY || baabb.maxY < aabb.minY ||
                baabb.minX > aabb.maxX || baabb.maxX < aabb.minX)
                return;
            if (aabb.maxZ < baabb.minZ) {
                float z2 = baabb.minZ - aabb.maxZ;
                if (z2 < z)
                    z = z2;
            }
        });
    } else if (z < 0) {
        world->getBlockBoxes(expanded, [&z, aabb](AABB const& baabb) {
            if (baabb.minY > aabb.maxY || baabb.maxY < aabb.minY ||
                baabb.minX > aabb.maxX || baabb.maxX < aabb.minX)
                return;
            if (aabb.minZ > baabb.maxZ) {
                float z2 = baabb.maxZ - aabb.minZ;
                if (z2 > z)
                    z = z2;
            }
        });
    }
    aabb.translate(0, 0, z);

    return {x, y, z};
}

void Entity::moveRelative(float x, float y, float z) {
    Vector3D basePos = getPos();
    Vector3D pos = checkCollisions(x, y, z);
    setPos(basePos.x + pos.x, basePos.y + pos.y, basePos.z + pos.z);
}

void Entity::setRot(float yaw, float pitch) {
    std::unique_lock<std::recursive_mutex> lock (generalMutex);
    this->yaw = yaw;
    this->pitch = pitch;
}

#ifdef SERVER
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
#endif

void Entity::damage(EntityDamageEvent& event) {
    Event::broadcast(event);

    if (event.isCancelled())
        return;

    if (event.getAttacker() != null) {
        Vector3D pos = event.getAttacker()->getPos();
        generalMutex.lock();
        knockBack(x - pos.x, z - pos.z, event.getKnockback());
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
    setMotion(motion);
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

void Entity::tickPhysics() {
    generalMutex.lock();
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    float delta = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - prevPhysicsTick).count();
    prevPhysicsTick = now;

    float blockSlipperiness = 0.6f;
    float motionReduction = blockSlipperiness * 0.91f;

    motion.x *= 0.98f;
    motion.y *= 0.98f;
    motion.z *= 0.98f;

    if (std::abs(motion.x) < 0.005f)
        motion.x = 0;
    if (std::abs(motion.y) < 0.005f)
        motion.y = 0;
    if (std::abs(motion.z) < 0.005f)
        motion.z = 0;

    moveRelative(motion.x, motion.y, motion.z);

    motion.y -= gravity;

    motion.x *= motionReduction;
    motion.z *= motionReduction;
    motion.y *= 0.98f;

    generalMutex.unlock();
}