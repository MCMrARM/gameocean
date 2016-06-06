#include "ChunkPtr.h"
#include "Chunk.h"

void ChunkPtr::setChunk(Chunk *chunk) {
    this->chunk = chunk;
    if (chunk != nullptr) {
        chunk->refMutex.lock();
        if (chunk->isDestroying) {
            chunk->refMutex.unlock();
            this->chunk = nullptr;
        } else {
            chunk->refCount++;
            chunk->refMutex.unlock();
        }
    } else {
        this->chunk = nullptr;
    }
}

void ChunkPtr::releaseChunk() {
    if (chunk != nullptr) {
        chunk->refMutex.lock();
        chunk->refCount--;
        if (chunk->refCount <= 0 && chunk->shouldDestroy() && !chunk->isDestroying) {
            chunk->isDestroying = true;
            chunk->refMutex.unlock();
            chunk->destroy();
        } else {
            chunk->refMutex.unlock();
        }
        chunk = nullptr;
    }
}