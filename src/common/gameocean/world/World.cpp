#include <gameocean/utils/Time.h>
#include "World.h"
#include "WorldProvider.h"
#ifdef SERVER
#include <gameocean/Player.h>
#endif

#include "../entity/ItemEntity.h"

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
    Chunk* ret = nullptr;
    if (provider != nullptr) {
        ret = provider->requestChunk(pos);
    }
    if (ret == nullptr && create) {
        ret = new Chunk(pos, true); // create an empty chunk
    }
    if (ret != nullptr)
        chunks[pos] = ret;
    chunkMutex.unlock();
    return ret;
}

void World::setTime(int time, bool stopped) {
    this->startTime = time;
    this->timeStopped = stopped;
    if (!stopped)
        this->startTimeMS = Time::now();
#ifdef SERVER
    broadcastTimeUpdate(time, stopped);
#endif
}

int World::getTime() {
    if (timeStopped)
        return startTime;
    long long now = Time::now();
    return startTime + (int) ((now - startTimeMS) / 50);
}

void World::dropItem(Vector3D pos, ItemInstance item) {
    new ItemEntity(*this, pos, item);
}

#ifdef SERVER

void World::broadcastBlockUpdate(BlockPos pos) {
    for (Player* p : getPlayers()) {
        p->sendBlockUpdate(pos);
    }
}

void World::broadcastTimeUpdate(int time, bool stopped) {
    for (Player* p : getPlayers()) {
        p->sendWorldTime(time, stopped);
    }
}

#endif