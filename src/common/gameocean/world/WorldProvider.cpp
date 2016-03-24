#include "WorldProvider.h"

#include "Chunk.h"

ChunkPtr ThreadedWorldProvider::requestChunk(ChunkPos pos) {
    Chunk* ret = new Chunk(world, pos);
    ret->ready = false;

    queueLock.lock();
    chunkQueue[pos] = true;
    queueLock.unlock();
    queueNotify.notify_all();
    return ret;
}

void ThreadedWorldProvider::run() {
    while(true) {
        if (chunkQueue.size() <= 0) {
            std::unique_lock<std::mutex> lock (queueNotifyMutex);
            queueNotify.wait(lock);
            if (shouldStop) return;
        }
        queueLock.lock();
        auto it = chunkQueue.begin();
        ChunkPos pos = it->first;
        chunkQueue.erase(it);
        queueLock.unlock();

        loadChunk(pos);
    }
}