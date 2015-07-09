#pragma once

#include <mutex>
#include <unordered_map>
#include "Chunk.h"
#include "ChunkPos.h"
#include "BlockPos.h"
#include "WorldProvider.h"

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


