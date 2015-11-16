#pragma once

#include <gameocean/common.h>
#include <mutex>
#include <set>
#include <gameocean/utils/Vector3D.h>

typedef long long EntityId;

class World;
class Chunk;
class Player;

class Entity {

protected:
    unsigned int typeId = 0;
    World& world;
    EntityId id;
    Chunk* chunk = null;

    bool closed = false;

    std::set<Player*> spawnedTo;

    std::recursive_mutex generalMutex;
    float x, y, z;

public:
    static EntityId currentId;

    Entity(World& world) : world(world) {
        id = Entity::currentId++;
    };
    Entity(World& world, float x, float y, float z) : Entity(world) { setPos(x, y, z); };
    virtual ~Entity() {};

    virtual void close();

    virtual std::string getTypeName() { return "Entity"; };

    inline EntityId getId() { return id; };
    inline World& getWorld() {
        generalMutex.lock();
        World& w = world;
        generalMutex.unlock();
        return w;
    };
    inline Chunk* getChunk() {
        generalMutex.lock();
        Chunk* c = chunk;
        generalMutex.unlock();
        return c;
    };

    virtual void setPos(float x, float y, float z);

    inline Vector3D getPos() {
        generalMutex.lock();
        Vector3D ret = Vector3D(x, y, z);
        generalMutex.unlock();
        return ret;
    };

    void updateViewers();

    void spawnTo(Player* player);
    void despawnFrom(Player* player);
    void spawnToAll();
    void despawnFromAll();

};


