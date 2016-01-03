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

class Chunk {

public:
    static Chunk* empty;

    ChunkPos pos;
    std::map<EntityId, Entity*> entities;
    std::set<std::shared_ptr<Tile>> tiles;
    std::set<Player*> usedBy;
    std::recursive_mutex mutex;

    byte blockId [16 * 16 * 128];
    NibbleArray<16 * 16 * 128 / 2> blockMeta;
    NibbleArray<16 * 16 * 128 / 2> blockSkylight;
    NibbleArray<16 * 16 * 128 / 2> blockLight;
    byte heightmap [16 * 16];
    uint32_t biomeColors [16 * 16];

    std::atomic<bool> ready;

    Chunk(ChunkPos pos) : pos(pos), ready(false) { };
    Chunk(int x, int z) : pos(x, z), ready(false) { };
    Chunk(ChunkPos pos, bool empty) : pos(pos), ready(empty) { if (empty) clear(); };
    Chunk(int x, int z, bool empty) : pos(x, z), ready(empty) { if (empty) clear(); };

    void clear() {
        memset(blockId, 0, sizeof(blockId));
        memset(blockMeta.array, 0, sizeof(blockMeta.array));
        memset(blockSkylight.array, 0, sizeof(blockSkylight.array));
        memset(blockLight.array, 0, sizeof(blockLight.array));
        memset(heightmap, 0, sizeof(heightmap));
        memset(biomeColors, 0, sizeof(biomeColors));
    };

    void addEntity(Entity* entity) {
        mutex.lock();
        entities[entity->getId()] = entity;
        mutex.unlock();
    };
    void removeEntity(Entity* entity) {
        mutex.lock();
        entities.erase(entity->getId());
        mutex.unlock();
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
        mutex.lock();
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
        mutex.unlock();
    }
#endif

};


