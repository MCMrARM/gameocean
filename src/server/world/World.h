#pragma once

#include <mutex>
#include <unordered_map>
#include "Chunk.h"
#include "ChunkPos.h"
#include "BlockPos.h"
#include "WorldBlock.h"
#include "WorldProvider.h"
#include "game/item/BlockVariant.h"

class WorldProvider;

class World {

private:
    std::string name;
    std::recursive_mutex chunkMutex;
    std::unordered_map<ChunkPos, Chunk*> chunks;
    WorldProvider* provider;

public:
    World(std::string name) : name(name) { };

    BlockPos spawn;

    inline std::string getName() { return name; };

    inline void setWorldProvider(WorldProvider* provider) {
        this->provider = provider;
    };

    void setBlock(int x, int y, int z, BlockId id, byte data) {
        Chunk* c = getChunkAt(x >> 4, z >> 4, false);
        if (c == null)
            return;
        c->mutex.lock();
        c->setBlock(x % 16, y, z % 16, id, data);
        c->mutex.unlock();
    };
    inline void setBlock(BlockPos pos, BlockId id, byte data) {
        setBlock(pos.x, pos.y, pos.z, id, data);
    };

    WorldBlock getBlock(int x, int y, int z) {
        Chunk* c = getChunkAt(x >> 4, z >> 4, false);
        if (c == null)
            return { 0, 0 };
        c->mutex.lock();
        WorldBlock r = c->getBlock(x % 16, y, z % 16);
        c->mutex.unlock();
        return r;
    };
    inline WorldBlock getBlock(BlockPos pos) {
        return getBlock(pos.x, pos.y, pos.z);
    };

    Chunk* getChunkAt(ChunkPos pos, bool create);
    inline Chunk* getChunkAt(ChunkPos pos) {
        return getChunkAt(pos, false);
    };
    inline Chunk* getChunkAt(int x, int z, bool create) {
        return getChunkAt(ChunkPos(x, z), create);
    };
    inline Chunk* getChunkAt(int x, int z) {
        return getChunkAt(x, z, false);
    };

    inline bool isChunkLoaded(ChunkPos pos) {
        chunkMutex.lock();
        bool ret = (chunks.count(pos) > 0);
        chunkMutex.unlock();
        return ret;
    };
    inline bool isChunkLoaded(int x, int z) {
        return isChunkLoaded(ChunkPos(x, z));
    };

    void loadSpawnTerrain() {
        int spawnChunkX = spawn.x >> 4;
        int spawnChunkZ = spawn.z >> 4;
        for (int x = spawnChunkX - 1; x <= spawnChunkX + 1; x++) {
            for (int z = spawnChunkZ - 1; z <= spawnChunkZ + 1; z++) {
                getChunkAt(spawnChunkX, spawnChunkZ, true);
            }
        }
    };

    void setChunk(Chunk* chunk) {
        chunkMutex.lock();
        if (isChunkLoaded(chunk->pos)) {
            Chunk* old = getChunkAt(chunk->pos);
            if (old != null)
                delete old;
        }
        chunks[chunk->pos] = chunk;
        chunkMutex.unlock();
    };

};


