#pragma once

#include <gameocean/common.h>
#include <mutex>
#include <set>
#include <gameocean/utils/Vector2D.h>
#include <gameocean/utils/Vector3D.h>

typedef long long EntityId;

class World;
class Chunk;
class Player;
class EntityDamageEvent;

class Entity {

protected:
    unsigned int typeId = 0;
    World* world;
    EntityId id;
    Chunk* chunk = null;

    bool closed = false;

    std::set<Player*> spawnedTo;

    std::recursive_mutex generalMutex;
    float x, y, z;
    float yaw, pitch;
    Vector3D motion;

    float hp, maxHp;

    void knockBack(float x, float z, float force);

public:
    static EntityId currentId;

    Entity(World& world) : world(&world) {
        id = Entity::currentId++;
    };
    Entity(World& world, float x, float y, float z) : Entity(world) { setPos(x, y, z); };
    virtual ~Entity() {};

    virtual void close();

    virtual const char* getTypeName() { return "Entity"; };

    inline EntityId getId() { return id; };
    inline World& getWorld() {
        std::unique_lock<std::recursive_mutex> lock (generalMutex);
        return *world;
    };
    inline Chunk* getChunk() {
        std::unique_lock<std::recursive_mutex> lock (generalMutex);
        return chunk;
    };

    virtual void setWorld(World& world, float x, float y, float z);
    virtual void setPos(float x, float y, float z);

    inline Vector3D getPos() {
        std::unique_lock<std::recursive_mutex> lock (generalMutex);
        return Vector3D(x, y, z);
    };

    virtual void setRot(float yaw, float pitch);

    inline Vector2D getRot() {
        std::unique_lock<std::recursive_mutex> lock (generalMutex);
        return Vector2D(yaw, pitch);
    };

    void updateViewers();

    void spawnTo(Player* player);
    void despawnFrom(Player* player);
    void spawnToAll();
    void despawnFromAll();

    inline float getHealth() {
        std::unique_lock<std::recursive_mutex> lock (generalMutex);
        return hp;
    };
    inline float getMaxHealth() {
        std::unique_lock<std::recursive_mutex> lock (generalMutex);
        return maxHp;
    };
    virtual void setHealth(float hp) {
        std::unique_lock<std::recursive_mutex> lock (generalMutex);
        if (hp <= 0) {
            kill();
            return;
        }
        this->hp = hp;
    };

    virtual void damage(EntityDamageEvent& event);

    virtual void kill() {
        close();
    };

    std::vector<Entity*> getNearbyEntities(float range);

};


