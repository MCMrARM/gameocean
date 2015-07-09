#include "World.h"
#include "WorldProvider.h"

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