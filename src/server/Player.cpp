#include "Player.h"

#include <iostream>
#include "world/World.h"
#include "command/Command.h"
#include "utils/StringUtils.h"

const std::string Player::TYPE_NAME = "Player";

void Player::close() {
    Entity::close();
    for (auto entry : sentChunks) {
        entry.second->setUsedBy(this, false);
    }
    for (auto entry : receivedChunks) {
        entry.second->setUsedBy(this, false);
    }
}

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

    Chunk* chunk = world.getChunkAt(pos, false);
    if (chunk != null)
        chunk->setUsedBy(this, true);
}

void Player::setSpawned() {
    if (spawned)
        return;

    spawned = true;
    chunkArrayMutex.lock();
    for (auto entry : sentChunks) {
        Chunk* c = entry.second;
        c->mutex.lock();
        for (auto e : c->entities) {
            e.second->spawnTo(this);
        }
        c->mutex.unlock();
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
        return true; // return true so the position won't be reverted

    setPos(x, y, z);
    return true;
}

void Player::sendQueuedChunks() {
    if (chunk == null)
        return;

    chunkArrayMutex.lock();
    int sent = 0;
    for (auto it = sendChunksQueue.begin(); it != sendChunksQueue.end(); ) {
        if (sent > server.sendChunksCount) {
            chunkArrayMutex.unlock();
            return;
        }
        ChunkPos pos = *it;
        if (sendChunk(pos.x, pos.z)) {
            it = sendChunksQueue.erase(it);
            sent++;
        } else {
            it++;
        }
    }
    chunkArrayMutex.unlock();

    updateTeleportState();
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
        entry.second->setUsedBy(this, false);
    }
    for (auto entry : remReceivedChunks) {
        receivedChunks.erase(entry.first);
        entry.second->setUsedBy(this, false);
    }

    chunkArrayMutex.unlock();
}

void Player::updateTeleportState() {
    if (!teleporting) return;

    chunkArrayMutex.lock();
    generalMutex.lock();
    int posX = chunk->pos.x;
    int posZ = chunk->pos.z;
    generalMutex.unlock();
    for (int x = posX - 1; x <= posX + 1; x++) {
        for (int z = posZ - 1; z <= posZ + 1; z++) {
            ChunkPos pos (x, z);
            if (receivedChunks.count(pos) <= 0) {
                chunkArrayMutex.unlock();
                return;
            }
        }
    }
    teleporting = false;
    chunkArrayMutex.unlock();

    generalMutex.lock();
    sendPosition(x, y, z);
    generalMutex.unlock();
}

void Player::processMessage(std::string text) {
    if (text[0] == '/') { // command
        std::vector<std::string> v = StringUtils::split(text.substr(1), " ");

        if (v.size() <= 0)
            return;

        std::string commandName = v[0];

        Command* c = Command::getCommand(commandName);
        if (c == null) {
            sendMessage("Command not found.");
            return;
        }
        c->process(*this, v);
    }
}