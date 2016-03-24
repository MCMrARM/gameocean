#include <gameocean/utils/Time.h>
#include "World.h"
#include "WorldProvider.h"
#ifdef SERVER
#include <gameocean/Player.h>
#endif

#include "../entity/ItemEntity.h"

World::World(std::string name) : name(name), physicsTickTask(*this), updateTickTask(*this) {
    setTime(5000, true);

    physicsTickTask.start();
    updateTickTask.start();
}

ChunkPtr World::getChunkAt(ChunkPos pos, bool create) {
    chunkMutex.lock();
    if (chunks.count(pos) > 0) {
        Chunk* c = chunks.at(pos);
        chunkMutex.unlock();
        return c;
    }
    ChunkPtr ret;
    if (provider != nullptr) {
        ret = provider->requestChunk(pos);
    }
    if (!ret && create) {
        ret = new Chunk(*this, pos, true); // create an empty chunk
    }
    if (ret)
        chunks[pos] = &*ret;
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
    std::shared_ptr<ItemEntity> ent (new ItemEntity(*this, item));
    ent->setPos(pos.x, pos.y, pos.z);
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