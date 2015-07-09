#pragma once

#include "common.h"

typedef long long EntityId;

class World;
class Chunk;

class Entity {

protected:
    World& world;
    EntityId id;
    Chunk* chunk = null;

public:
    static EntityId currentId;

    float x, y, z;

    Entity(World& world) : world(world) {
        id = Entity::currentId++;
    };
    Entity(World& world, float x, float y, float z) : Entity(world) { setPos(x, y, z); };
    ~Entity();

    inline World& getWorld() { return world; };
    inline EntityId getId() { return id; };
    inline Chunk* getChunk() { return chunk; };

    virtual void setPos(float x, float y, float z);

};


