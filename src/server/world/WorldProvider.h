#pragma once

#include <unordered_map>
#include <condition_variable>
#include "ChunkPos.h"
#include "utils/Thread.h"
class World;
class Chunk;

class WorldProvider {

protected:
    World& world;

public:
    WorldProvider(World& world) : world(world) {};

    virtual Chunk* requestChunk(ChunkPos pos) = 0;

};

class ThreadedWorldProvider : public WorldProvider, public Thread {

protected:
    bool shouldStop = false;
    std::mutex queueLock;
    std::mutex queueNotifyMutex;
    std::condition_variable queueNotify;
    std::unordered_map<ChunkPos, bool> chunkQueue;

    virtual void run();

    virtual void loadChunk(ChunkPos pos) = 0;

public:
    ThreadedWorldProvider(World& world) : WorldProvider(world) {};

    virtual Chunk* requestChunk(ChunkPos pos);

    virtual void stop() { shouldStop = true; queueNotify.notify_all(); };

};

