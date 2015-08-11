#pragma once

#include "common.h"
#include "ChunkPos.h"
#include <map>
#include <cstring>
#include "../Entity.h"
#include "Block.h"
#include "utils/NibbleArray.h"

class Chunk {

public:
    static Chunk* empty;

    ChunkPos pos;
    std::map<EntityId, Entity*> entities;

    byte blockId [16 * 16 * 128];
    NibbleArray<16 * 16 * 128 / 2> blockMeta;
    NibbleArray<16 * 16 * 128 / 2> blockSkylight;
    NibbleArray<16 * 16 * 128 / 2> blockLight;
    byte heightmap [16 * 16];
    uint32_t biomeColors [16 * 16];

    bool ready = false;

    Chunk(ChunkPos pos) : pos(pos) { };
    Chunk(int x, int z) : pos(x, z) { };
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

    inline void addEntity(Entity* entity) {
        entities[entity->getId()] = entity;
    };
    inline void removeEntity(Entity* entity) {
        entities.erase(entity->getId());
    };

    inline int getBlockPos(int x, int y, int z) {
        return ((x << 11) | (z << 7) | y);
    };

    void setBlock(int x, int y, int z, BlockId id, byte data) {
        blockId[getBlockPos(x, y, z)] = id;
        blockMeta.set(getBlockPos(x, y, z), data);
        blockLight.set(getBlockPos(x, y, z), 15);
        blockSkylight.set(getBlockPos(x, y, z), 0);

        if (id != 0 && heightmap[(z << 4) | x] < y) {
            heightmap[(z << 4) | x] = y;
        } else if (heightmap[(z << 4) | x] == y) {
            heightmap[(z << 4) | x] = y - 1;
        }
    };

};


