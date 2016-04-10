#pragma once

#include <gameocean/common.h>
#include <map>
#include <mutex>
#include <atomic>
#include <cstring>
#include <set>
#include "ChunkPos.h"
#include "WorldBlock.h"
#include "../entity/Entity.h"
#ifdef SERVER
#include <gameocean/Player.h>
#endif
#include <gameocean/item/BlockVariant.h>
#include <gameocean/utils/NibbleArray.h>

class Tile;
class World;
class ChunkPtr;

/**
 * This is the basic Chunk class. You should avoid using it, if possible.
 *
 * If you need to store a reference to a chunk, please use a ChunkPtr. If you are given a reference, you can use the
 * ChunkPtr constructor to get this type of pointer.
 */
class Chunk {

protected:
    friend class ChunkPtr;
    friend class World;

    std::mutex refMutex;
    std::atomic<bool> isDestroying;
    /** When this chunk is detached from World; this will cause this Chunk to be delete'd as soon as all references are lost. */
    std::atomic<bool> markedDead;
    std::atomic<int> refCount;

    /** When ref count goes to 0, destroy this chunk */
    void destroy();

    bool shouldDestroy();

public:
    World& world;
    const ChunkPos pos;
    std::map<EntityId, std::shared_ptr<Entity>> entities;
    std::set<std::shared_ptr<Tile>> tiles;
    std::set<Player*> usedBy;
    std::recursive_mutex mutex;
    std::recursive_mutex entityMutex;
    std::mutex tilesMutex;

    byte blockId [16 * 16 * 128];
    NibbleArray<16 * 16 * 128 / 2> blockMeta;
    NibbleArray<16 * 16 * 128 / 2> blockSkylight;
    NibbleArray<16 * 16 * 128 / 2> blockLight;
    byte heightmap [16 * 16];
    uint32_t biomeColors [16 * 16];

    std::atomic<bool> ready;

    Chunk(World& world, ChunkPos pos) : world(world), pos(pos), ready(false), refCount(0), isDestroying(false), markedDead(false) { };
    Chunk(World& world, int x, int z) : world(world), pos(x, z), ready(false), refCount(0), isDestroying(false), markedDead(false) { };
    Chunk(World& world, ChunkPos pos, bool empty) : world(world), pos(pos), ready(empty), refCount(0), isDestroying(false), markedDead(false) { if (empty) clear(); };
    Chunk(World& world, int x, int z, bool empty) : world(world), pos(x, z), ready(empty), refCount(0), isDestroying(false), markedDead(false) { if (empty) clear(); };

    void clear() {
        memset(blockId, 0, sizeof(blockId));
        memset(blockMeta.array, 0, sizeof(blockMeta.array));
        memset(blockSkylight.array, 0, sizeof(blockSkylight.array));
        memset(blockLight.array, 0, sizeof(blockLight.array));
        memset(heightmap, 0, sizeof(heightmap));
        memset(biomeColors, 0, sizeof(biomeColors));
    };

    void setLoaded();

    void addEntity(std::shared_ptr<Entity> entity) {
        entityMutex.lock();
        entities[entity->getId()] = entity;
        entityMutex.unlock();
    };
    void removeEntity(Entity* entity) {
        entityMutex.lock();
        entities.erase(entity->getId());
        entityMutex.unlock();
    };

    inline int getBlockPos(int x, int y, int z) {
        return ((y << 8) | (z << 4) | x);
    };

    void setBlock(int x, int y, int z, BlockId id, byte data) {
        int pos = getBlockPos(x, y, z);
        blockId[pos] = id;
        blockMeta.set(pos, data);
        blockLight.set(pos, 15);
        blockSkylight.set(pos, 0);

        if (id != 0 && heightmap[(z << 4) | x] < y) {
            heightmap[(z << 4) | x] = y;
        } else if (heightmap[(z << 4) | x] == y) {
            heightmap[(z << 4) | x] = y - 1;
        }
    };

    WorldBlock getBlock(int x, int y, int z) {
        int pos = getBlockPos(x, y, z);
        return { blockId[pos], blockMeta[pos] };
    }

    std::shared_ptr<Tile> getTile(int x, int y, int z);

#ifdef SERVER
    inline void setUsedBy(Player* player, bool used) {
        entityMutex.lock();
        if (used) {
            if (usedBy.count(player) <= 0)
                usedBy.insert(player);

            if (player->hasSpawned()) {
                for (auto e : entities) {
                    e.second->spawnTo(player);
                }
            }
        } else {
            if (usedBy.count(player) > 0)
                usedBy.erase(player);

            for (auto e : entities) {
                e.second->despawnFrom(player);
            }
        }
        entityMutex.unlock();
    }
#endif

};


