#include "Chunk.h"

#include "World.h"
#include "tile/Tile.h"

std::shared_ptr<Tile> Chunk::getTile(int x, int y, int z) {
    std::unique_lock<std::mutex> lock (tilesMutex);
    BlockPos p = {x, y, z};
    for (std::shared_ptr<Tile> const &t : tiles) {
        if (t->getPos() == p)
            return t;
    }
    return nullptr;
}

void Chunk::destroy() {
    if (markedDead) {
        delete this;
        return;
    }
    world.notifyChunkDead(this);
}

bool Chunk::shouldDestroy() {
    if (markedDead)
        return true;
    if (world.getChunkUnloadPolicy() == World::ChunkUnloadPolicy::AS_SOON_AS_POSSIBLE)
        return true;
    return false;
}

void Chunk::setLoaded() {
    ready = true;
    world.markChunkLoaded(this);
}