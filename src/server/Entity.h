#pragma once

#include "common.h"
#include <mutex>
#include "utils/Vector3D.h"

typedef long long EntityId;

class World;
class Chunk;

class Entity {

protected:
    World& world;
    EntityId id;
    Chunk* chunk = null;

    std::mutex generalMutex;
    float x, y, z;

public:
    static EntityId currentId;

    Entity(World& world) : world(world) {
        id = Entity::currentId++;
    };
    Entity(World& world, float x, float y, float z) : Entity(world) { setPos(x, y, z); };
    ~Entity();

    inline World& getWorld() { return world; };
    inline EntityId getId() { return id; };
    inline Chunk* getChunk() { return chunk; };

    virtual void setPos(float x, float y, float z);

    inline Vector3D getPos() {
        generalMutex.lock();
        Vector3D ret = Vector3D(x, y, z);
        generalMutex.unlock();
        return ret;
    };

};


