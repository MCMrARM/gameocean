#pragma once

#include <mutex>
#include <unordered_map>
#include "Chunk.h"
#include "ChunkPos.h"
#include "BlockPos.h"
#include "WorldBlock.h"
#include "WorldProvider.h"
#include <gameocean/game/item/BlockVariant.h>

class WorldProvider;
class Player;

class World {

private:
    std::string name;
    std::recursive_mutex chunkMutex;
    std::unordered_map<ChunkPos, Chunk*> chunks;
    std::set<Player*> players;
    WorldProvider* provider;
    int startTime;
    long long startTimeMS;
    bool timeStopped = false;

public:
    World(std::string name);

    BlockPos spawn;

    inline std::string getName() { return name; };

    inline void setWorldProvider(WorldProvider* provider) {
        this->provider = provider;
    };

    void setBlock(int x, int y, int z, BlockId id, byte data) {
        Chunk* c = getChunkAt(x >> 4, z >> 4, false);
        if (c == null)
            return;
        std::lock_guard<std::recursive_mutex> guard (c->mutex);
        c->setBlock(x & 0xf, y, z & 0xf, id, data);
#ifdef SERVER
        broadcastBlockUpdate({x, y, z});
#endif
    };
    inline void setBlock(BlockPos pos, BlockId id, byte data) {
        setBlock(pos.x, pos.y, pos.z, id, data);
    };

    WorldBlock getBlock(int x, int y, int z) {
        Chunk* c = getChunkAt(x >> 4, z >> 4, false);
        if (c == null)
            return { 0, 0 };
        std::lock_guard<std::recursive_mutex> guard (c->mutex);
        return c->getBlock(x & 0xf, y, z & 0xf);
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
        std::lock_guard<std::recursive_mutex> guard (chunkMutex);
        return (chunks.count(pos) > 0);
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

    void addPlayer(Player* player) {
        chunkMutex.lock();
        players.insert(player);
        chunkMutex.unlock();
    };

    void removePlayer(Player* player) {
        chunkMutex.lock();
        players.erase(player);
        chunkMutex.unlock();
    };

    std::set<Player*> getPlayers() {
        std::lock_guard<std::recursive_mutex> guard (chunkMutex);
        return players;
    };

    void setTime(int time, bool stopped);

    int getTime();

    inline bool isTimeStopped() { return timeStopped; };

    void setTimeStopped(bool stopped) {
        setTime(getTime(), stopped);
    }

#ifdef SERVER
    void broadcastBlockUpdate(BlockPos pos);

    void broadcastTimeUpdate(int time, bool stopped);
#endif

};


