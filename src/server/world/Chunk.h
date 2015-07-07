#pragma once

#include "common.h"
#include "ChunkPos.h"
#include <map>
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

    Chunk(ChunkPos pos) : pos(pos) {};
    Chunk(int x, int z) : pos({x, z}) {};

    inline void addEntity(Entity* entity) {
        entities[entity->getId()] = entity;
    };
    inline void removeEntity(Entity* entity) {
        entities.erase(entity->getId());
    };

    inline int getBlockPos(int x, int y, int z) {
        return ((z << 11) | (x << 7) | y);
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


