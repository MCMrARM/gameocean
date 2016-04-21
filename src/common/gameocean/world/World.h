#pragma once

#include <mutex>
#include <unordered_map>
#include <memory>
#include "Chunk.h"
#include "ChunkPos.h"
#include "ChunkPtr.h"
#include "BlockPos.h"
#include "WorldBlock.h"
#include "WorldProvider.h"
#include <gameocean/item/BlockVariant.h>
#include "../math/AABB.h"
#include "../model/Model.h"
#include "EntityPhysicsTickTask.h"
#include "EntityUpdateTickTask.h"
#include "WorldListener.h"

class WorldProvider;
class WorldListener;
class Player;
class Tile;

class World {

public:
    enum class ChunkUnloadPolicy {
        AS_SOON_AS_POSSIBLE, NEVER
    };

private:
    std::string name;
    std::recursive_mutex chunkMutex;
    std::recursive_mutex miscMutex;
    std::unordered_map<ChunkPos, Chunk*> chunks;
    std::set<Player*> players;
    std::set<WorldListener*> listeners;
    std::vector<ChunkPtr> spawnTerrain;
    WorldProvider* provider;
    int startTime;
    long long startTimeMS;
    bool timeStopped = false;
    EntityPhysicsTickTask physicsTickTask;
    EntityUpdateTickTask updateTickTask;
    ChunkUnloadPolicy unloadPolicy = ChunkUnloadPolicy::AS_SOON_AS_POSSIBLE;

public:
    World(std::string name);

    ~World() {
        for (WorldListener* listener : listeners) {
            listener->onWorldDestroy(*this);
        }
    }

    BlockPos spawn;

    inline std::string getName() { return name; };

    inline void addListener(WorldListener* listener) {
        listeners.insert(listener);
    }
    inline void removeListener(WorldListener* listener) {
        listeners.erase(listener);
    }

    inline void setWorldProvider(WorldProvider* provider) {
        this->provider = provider;
    }

    inline void setChunkUnloadPolicy(ChunkUnloadPolicy policy) {
        std::unique_lock<std::recursive_mutex> lock (miscMutex);
        unloadPolicy = policy;
    }
    inline ChunkUnloadPolicy getChunkUnloadPolicy() {
        std::unique_lock<std::recursive_mutex> lock (miscMutex);
        return unloadPolicy;
    }

    void setBlock(int x, int y, int z, BlockId id, byte data) {
        ChunkPtr c = getChunkAt(x >> 4, z >> 4, false);
        if (!c)
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
        ChunkPtr c = getChunkAt(x >> 4, z >> 4, false);
        if (!c)
            return { 0, 0 };
        std::lock_guard<std::recursive_mutex> guard (c->mutex);
        return c->getBlock(x & 0xf, y, z & 0xf);
    };
    inline WorldBlock getBlock(BlockPos pos) {
        return getBlock(pos.x, pos.y, pos.z);
    };

    inline std::shared_ptr<Tile> getTile(int x, int y, int z) {
        ChunkPtr c = getChunkAt(x >> 4, z >> 4, false);
        if (!c)
            return nullptr;
        std::lock_guard<std::recursive_mutex> guard (c->mutex);
        return c->getTile(x, y, z);
    };
    inline std::shared_ptr<Tile> getTile(BlockPos pos) {
        return getTile(pos.x, pos.y, pos.z);
    };

    ChunkPtr getChunkAt(ChunkPos pos, bool create);
    inline ChunkPtr getChunkAt(ChunkPos pos) {
        return getChunkAt(pos, false);
    };
    inline ChunkPtr getChunkAt(int x, int z, bool create) {
        return getChunkAt(ChunkPos(x, z), create);
    };
    inline ChunkPtr getChunkAt(int x, int z) {
        return getChunkAt(x, z, false);
    };

    inline bool isChunkLoaded(ChunkPos pos) {
        std::lock_guard<std::recursive_mutex> guard (chunkMutex);
        return (chunks.count(pos) > 0);
    };
    inline bool isChunkLoaded(int x, int z) {
        return isChunkLoaded(ChunkPos(x, z));
    };

    /**
     * @deprecated
     * Please use getChunkPtrs instead if possible.
     */
    inline std::unordered_map<ChunkPos, Chunk*> getChunks() {
        std::lock_guard<std::recursive_mutex> guard (chunkMutex);
        return chunks;
    };
    inline std::vector<ChunkPtr> getChunkPtrs() {
        std::lock_guard<std::recursive_mutex> guard (chunkMutex);
        std::vector<ChunkPtr> ptrs;
        ptrs.reserve(chunks.size());
        for (auto chunk : chunks) {
            if (!chunk.second->isDestroying)
                ptrs.push_back(ChunkPtr(chunk.second));
        }
        return ptrs;
    }

    void loadSpawnTerrain() {
        std::lock_guard<std::recursive_mutex> guard (chunkMutex);
        if (spawnTerrain.size() > 0)
            return;
        int spawnChunkX = spawn.x >> 4;
        int spawnChunkZ = spawn.z >> 4;
        for (int x = spawnChunkX - 1; x <= spawnChunkX -1; x++) {
            for (int z = spawnChunkZ - 1; z <= spawnChunkZ -1; z++) {
                spawnTerrain.push_back(getChunkAt(spawnChunkX, spawnChunkZ, true));
            }
        }
    };

    void setChunk(Chunk* chunk) {
        chunkMutex.lock();
        if (isChunkLoaded(chunk->pos)) {
            removeChunk(chunks[chunk->pos]);
        }
        chunks[chunk->pos] = chunk;
        chunkMutex.unlock();

        for (WorldListener* listener : listeners) {
            listener->onChunkAdded(*this, *chunk);
        }
    };

    void markChunkLoaded(Chunk* chunk) {
        if (chunk == nullptr || &*getChunkAt(chunk->pos, false) != chunk)
            return;
        for (WorldListener* listener : listeners) {
            listener->onChunkLoaded(*this, *chunk);
        }
    };

    bool removeChunk(Chunk* chunk) {
        chunkMutex.lock();
        if (chunk == nullptr || !isChunkLoaded(chunk->pos) || chunks[chunk->pos] != chunk) {
            chunkMutex.unlock();
            return false;
        }
        chunk->markedDead = true;
        chunks.erase(chunk->pos);
        chunkMutex.unlock();
        for (WorldListener* listener : listeners) {
            listener->onChunkUnloaded(*this, *chunk);
        }
        return true;
    };

    void notifyChunkDead(Chunk* chunk) {
        chunkMutex.lock();
        if (unloadPolicy == ChunkUnloadPolicy::AS_SOON_AS_POSSIBLE) {
            removeChunk(chunk);
            delete chunk;
        }
        chunkMutex.unlock();
    }

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

    void dropItem(Vector3D pos, ItemInstance item);

    template <typename T>
    void getBlockBoxes(AABB aabb, T callback) {
        int minX = (int) std::floor(aabb.minX);
        int maxX = (int) std::ceil(aabb.maxX);
        int minY = std::max((int) std::floor(aabb.minY), 0);
        int maxY = std::max((int) std::ceil(aabb.maxY), 0);
        int minZ = (int) std::floor(aabb.minZ);
        int maxZ = (int) std::ceil(aabb.maxZ);
        for (int x = minX; x <= maxX; x++) {
            for (int y = minY; y <= maxY; y++) {
                for (int z = minZ; z <= maxZ; z++) {
                    BlockVariant* variant = getBlock(x, y, z).getBlockVariant();
                    if (variant != nullptr) {
                        for (AABB a : variant->model->aabbs) {
                            a.translate(x, y, z);
                            callback(a);
                        }
                    }
                }
            }
        }
    }

    template <typename T>
    void getNearbyEntities(AABB aabb, T callback) {
        int minX = (int) std::floor(aabb.minX) >> 4;
        int maxX = (int) std::ceil(aabb.maxX) >> 4;
        int minZ = (int) std::floor(aabb.minZ) >> 4;
        int maxZ = (int) std::ceil(aabb.maxZ) >> 4;
        for (int x = minX; x <= maxX; x++) {
            for (int z = minZ; z <= maxZ; z++) {
                ChunkPtr chunk = getChunkAt(x, z, false);
                if (!chunk)
                    continue;
                chunk->entityMutex.lock();
                for (auto const& pair : chunk->entities) {
                    std::shared_ptr<Entity> ent = pair.second;
                    if (ent->getAABB().intersects(aabb))
                        callback(ent);
                }
                chunk->entityMutex.unlock();
            }
        }
    }

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


