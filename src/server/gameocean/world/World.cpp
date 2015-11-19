#include <gameocean/utils/Time.h>
#include "World.h"
#include "WorldProvider.h"

World::World(std::string name) : name(name) {
    setTime(5000, true);
}

Chunk* World::getChunkAt(ChunkPos pos, bool create) {
    chunkMutex.lock();
    if (chunks.count(pos) > 0) {
        Chunk* c = chunks.at(pos);
        chunkMutex.unlock();
        return c;
    }
    Chunk* ret = null;
    if (provider != null) {
        ret = provider->requestChunk(pos);
    }
    if (ret == null && create) {
        ret = new Chunk(pos, true); // create an empty chunk
    }
    if (ret != null)
        chunks[pos] = ret;
    chunkMutex.unlock();
    return ret;
}

void World::setTime(int time, bool stopped) {
    this->startTime = time;
    this->timeStopped = stopped;
    if (!stopped)
        this->startTimeMS = Time::now();
    broadcastTimeUpdate(time, stopped);
}

int World::getTime() {
    if (timeStopped)
        return startTime;
    long long now = Time::now();
    return startTime + (int) ((now - startTimeMS) / 50);
}