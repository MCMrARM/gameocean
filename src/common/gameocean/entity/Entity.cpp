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
    generalMutex.unlock();
    if (chunk) {
        chunk->removeEntity(this);
    }
}

void Entity::setWorld(World& world, float x, float y, float z) {
    generalMutex.lock();
    this->world = &world;
    this->x = x;
    this->y = y;
    this->z = z;

    int newChunkX = ((int) x) >> 4;
    int newChunkZ = ((int) z) >> 4;
    if (!chunk) {
        chunk = world.getChunkAt(newChunkX, newChunkZ);
        chunk->addEntity(shared_from_this());
        updateViewers();
    } else {
        std::shared_ptr<Entity> ptr (shared_from_this());
        chunk->removeEntity(this);
        chunk = world.getChunkAt(newChunkX, newChunkZ, true);
        chunk->addEntity(ptr);
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
    if (!chunk) {
        chunk = world->getChunkAt(newChunkX, newChunkZ);
        chunk->addEntity(shared_from_this());
        updateViewers();
    } else if (newChunkX != chunk->pos.x || newChunkZ != chunk->pos.z) {
        std::shared_ptr<Entity> ptr (shared_from_this());
        chunk->removeEntity(this);
        chunk = world->getChunkAt(newChunkX, newChunkZ, true);
        chunk->addEntity(ptr);
        updateViewers();
    }
#ifdef SERVER
    for (Player* p : spawnedTo) {
        p->updateEntityPos(this);
    }
#endif

    updateOnGround();
    generalMutex.unlock();
}

float Entity::getExistenceTime() {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - spawnedTime).count();
}

void Entity::updateOnGround() {
    AABB aabb = getAABB();
    AABB aabbCheck = aabb;
    aabbCheck.maxY = aabb.minY + 0.5f;
    aabbCheck.minY -= 0.5f;
    bool onGround = false;
    world->getBlockBoxes(aabbCheck, [&onGround, aabb](AABB const& baabb) {
        if (baabb.minX > aabb.maxX || baabb.maxX < aabb.minX ||
                baabb.minZ > aabb.maxZ || baabb.maxZ < aabb.minZ ||
                baabb.minY > aabb.maxY || baabb.maxY < aabb.minY)
            return;
        onGround = true;
    });
    generalMutex.lock();
    bool prevOnGround = this->onGround;
    this->onGround = onGround;
    generalMutex.unlock();
    if (onGround && !prevOnGround) {
        float dist = fallStart - aabb.minY;
        if (dist > 0.f)
            damageFall(dist);
    } else if (!onGround && prevOnGround) {
        fallStart = aabb.minY;
    }
}

Vector3D Entity::checkCollisions(float x, float y, float z) {
    generalMutex.lock();
    AABB aabb = this->aabb;
    float acceptance = collisionInBlockAcceptance;
    generalMutex.unlock();

    AABB expanded = aabb;
    expanded.add(0, y, 0);
    if (y > 0) {
        world->getBlockBoxes(expanded, [&y, expanded, acceptance](AABB const& baabb) {
            if (baabb.minX > expanded.maxX - acceptance || baabb.maxX < expanded.minX + acceptance ||
                baabb.minZ > expanded.maxZ - acceptance || baabb.maxZ < expanded.minZ + acceptance)
                return;
            if (expanded.maxY - acceptance > baabb.minY && expanded.minY < baabb.minY) {
                float y2 = baabb.minY - expanded.maxY;
                if (y2 < y)
                    y = y2;
            }
        });
    } else if (y < 0) {
        world->getBlockBoxes(expanded, [&y, expanded, acceptance](AABB const& baabb) {
            if (baabb.minX > expanded.maxX - acceptance || baabb.maxX < expanded.minX + acceptance ||
                baabb.minZ > expanded.maxZ - acceptance || baabb.maxZ < expanded.minZ + acceptance)
                return;
            if (expanded.minY + acceptance < baabb.maxY && expanded.maxY > baabb.maxY) {
                float y2 = baabb.maxY - expanded.minY;
                if (y2 > y)
                    y = y2;
            }
        });
    }
    aabb.translate(0, y, 0);
    expanded = aabb;
    expanded.add(x, 0, 0);
    if (x > 0) {
        world->getBlockBoxes(expanded, [&x, expanded, aabb, acceptance](AABB const& baabb) {
            if (baabb.minY > expanded.maxY - acceptance || baabb.maxY <= expanded.minY + acceptance ||
                baabb.minZ > expanded.maxZ - acceptance || baabb.maxZ < expanded.minZ + acceptance)
                return;
            if (expanded.maxX - acceptance > baabb.minX && expanded.minX < baabb.minX) {
                float x2 = baabb.minX - aabb.maxX;
                std::cout << "B" << x << " " << x2 << " " << baabb.minX << " " << expanded.maxX << "\n";
                if (x2 < x)
                    x = x2;
            }
        });
    } else if (x < 0) {
        world->getBlockBoxes(expanded, [&x, expanded, aabb, acceptance](AABB const& baabb) {
            if (baabb.minY > expanded.maxY - acceptance || baabb.maxY <= expanded.minY + acceptance ||
                baabb.minZ > expanded.maxZ - acceptance || baabb.maxZ < expanded.minZ + acceptance)
                return;
            if (expanded.minX + acceptance < baabb.maxX && expanded.maxX > baabb.maxX) {
                float x2 = baabb.maxX - aabb.minX;
                std::cout << x << " " << x2 << " " << baabb.maxX << " " << expanded.minX << "\n";
                if (x2 > x)
                    x = x2;
            }
        });
    }
    aabb.translate(x, 0, 0);
    expanded = aabb;
    expanded.add(0, 0, z);
    if (z > 0) {
        world->getBlockBoxes(expanded, [&z, expanded, acceptance](AABB const& baabb) {
            if (baabb.minY > expanded.maxY - acceptance || baabb.maxY <= expanded.minY + acceptance ||
                baabb.minX > expanded.maxX - acceptance || baabb.maxX < expanded.minX + acceptance)
                return;
            if (expanded.maxZ - acceptance > baabb.minZ && expanded.minZ < baabb.minZ) {
                float z2 = baabb.minZ - expanded.maxZ;
                if (z2 < z)
                    z = z2;
            }
        });
    } else if (z < 0) {
        world->getBlockBoxes(expanded, [&z, expanded, acceptance](AABB const& baabb) {
            if (baabb.minY > expanded.maxY - acceptance || baabb.maxY <= expanded.minY + acceptance ||
                baabb.minX > expanded.maxX - acceptance || baabb.maxX < expanded.minX + acceptance)
                return;
            if (expanded.minZ + acceptance < baabb.maxZ && expanded.maxZ > baabb.maxZ) {
                float z2 = baabb.maxZ - expanded.minZ;
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
    if (pos.x == 0 && pos.y == 0 && pos.z == 0)
        return;
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
    chunk->entityMutex.lock();
    for (Player* p : chunk->usedBy) {
        if (despawnFromPlayers.count(p) > 0) {
            despawnFromPlayers.erase(p);
        } else {
            spawnTo(p);
        }
    }
    chunk->entityMutex.unlock();
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
    chunk->entityMutex.lock();
    for (Player* p : chunk->usedBy) {
        spawnTo(p);
    }
    chunk->entityMutex.unlock();
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

    if (event.getAttacker() != nullptr) {
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

void Entity::damageFall(float distance) {
    distance = std::round(distance);
    if (distance > 3.f) {
        EntityDamageEvent ev (*this, (distance - 3.f), EntityDamageEvent::DamageSource::FALL, nullptr, 0.f);
        damage(ev);
    }
}

std::vector<std::shared_ptr<Entity>> Entity::getNearbyEntities(float range) {
    Vector3D myPos = getPos();
    World& world = getWorld();
    std::vector<std::shared_ptr<Entity>> ret;
    AABB aabb = {myPos.x, myPos.y, myPos.z, myPos.x, myPos.y, myPos.z};
    aabb.expand(range, range, range);
    for (int chunkX = (((int) myPos.x - (int) ceil(range)) >> 4); chunkX <= (((int) myPos.x + (int) ceil(range)) >> 4); chunkX++) {
        for (int chunkZ = (((int) myPos.z - (int) ceil(range)) >> 4); chunkZ <= (((int) myPos.z + (int) ceil(range)) >> 4); chunkZ++) {
            if (world.isChunkLoaded(chunkX, chunkZ)) {
                ChunkPtr chunk = world.getChunkAt(chunkX, chunkZ, false);
                chunk->entityMutex.lock();
                for (auto const& e : chunk->entities) {
                    std::shared_ptr<Entity> ent = e.second;
                    if (&*ent == this)
                        continue;
                    if (aabb.intersects(ent->getAABB()))
                        ret.push_back(ent);
                }
                chunk->entityMutex.unlock();
            }
        }
    }
    return ret;
}

void Entity::update() {

}

void Entity::tickPhysics() {
    generalMutex.lock();
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    float delta = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - prevPhysicsTick).count();
    prevPhysicsTick = now;

    float multiplier = delta / (1.f / 20);

    float dragMultiplier = std::pow(1.f - drag, multiplier);

    float blockSlipperiness = onGround ? 0.6f : 1.f;
    float motionReduction = std::pow(blockSlipperiness * (1.f - drag), multiplier);

    motion.y -= gravity * multiplier;

    motion.x *= motionReduction;
    motion.z *= motionReduction;
    motion.y *= dragMultiplier;


    motion.x *= dragMultiplier;
    motion.y *= dragMultiplier;
    motion.z *= dragMultiplier;

    if (std::abs(motion.x) < 0.005f)
        motion.x = 0;
    if (std::abs(motion.y) < 0.005f)
        motion.y = 0;
    if (std::abs(motion.z) < 0.005f)
        motion.z = 0;

    moveRelative(motion.x, motion.y, motion.z);

    generalMutex.unlock();
}