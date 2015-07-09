#include "Player.h"

#include "world/World.h"

bool Player::sendChunk(int x, int z) {
    ChunkPos pos (x, z);
    Chunk* chunk = world.getChunkAt(pos, true);
    if (chunk == null || !chunk->ready)
        return false;
    chunkArrayMutex.lock();
    sentChunks[pos] = chunk;
    chunkArrayMutex.unlock();
    return true;
}

void Player::receivedChunk(int x, int z) {
    chunkArrayMutex.lock();
    ChunkPos pos (x, z);
    if (sentChunks.count(pos) > 0) {
        receivedChunks[pos] = world.getChunkAt(pos);
    }
    chunkArrayMutex.unlock();
}

void Player::setPos(float x, float y, float z) {
    Chunk* oldChunk = chunk;
    Entity::setPos(x, y, z);

    if (oldChunk != chunk) {
        shouldUpdateChunkQueue = true;
    }
}

void Player::teleport(float x, float y, float z) {
    teleporting = true;
    setPos(x, y, z);
}

bool Player::tryMove(float x, float y, float z) {
    if (!spawned)
        return false;
    if (teleporting)
        return false;

    setPos(x, y, z);
    return true;
}

void Player::sendQueuedChunks() {
    if (chunk == null)
        return;

    int sent = 0;
    for (auto it = sendChunksQueue.begin(); it != sendChunksQueue.end(); ) {
        if (sent > server.sendChunksCount) return;
        ChunkPos pos = *it;
        if (sendChunk(pos.x, pos.z)) {
            it = sendChunksQueue.erase(it);
            sent++;
        } else {
            it++;
        }
    }
}

void Player::updateChunkQueue() {
    if (chunk == null)
        return;

    if (!shouldUpdateChunkQueue) {
        return;
    }
    shouldUpdateChunkQueue = false;

    chunkArrayMutex.lock();
    sendChunksQueue.clear();

    std::unordered_map<ChunkPos, Chunk*> remSentChunks = sentChunks;
    std::unordered_map<ChunkPos, Chunk*> remReceivedChunks = receivedChunks;

    int dir = 0;
    int len = 0;
    int x = 0;
    int z = 0;
    for (int i = 0; i < viewChunks; i++) {
        ChunkPos currentPos (chunk->pos.x + x, chunk->pos.z + z);
        if (sentChunks.count(currentPos) <= 0 && receivedChunks.count(currentPos) <= 0) {
            sendChunksQueue.push_back(currentPos);
        }
        remSentChunks.erase(currentPos);
        remReceivedChunks.erase(currentPos);

        if (dir == 0) {
            x++;
            if (x >= len) dir++;
        } else if (dir == 1) {
            z++;
            if (z >= len) dir++;
        } else if (dir == 2) {
            x--;
            if (-x >= len) dir++;
        } else if (dir == 3) {
            z--;
            if (-z >= len) {
                dir = 0;
                len++;
            }
        }
    }

    for (auto entry : remSentChunks) {
        sentChunks.erase(entry.first);
    }
    for (auto entry : remReceivedChunks) {
        receivedChunks.erase(entry.first);
    }
    chunkArrayMutex.unlock();
}