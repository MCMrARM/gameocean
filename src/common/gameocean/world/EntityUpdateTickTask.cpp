#include "EntityUpdateTickTask.h"

#include "World.h"

int EntityUpdateTickTask::tickRate = 50;

void EntityUpdateTickTask::run() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(EntityUpdateTickTask::tickRate));

        if (shouldStop)
            break;

        for (auto &chunk : world.getChunkPtrs()) {
            chunk->entityMutex.lock();
            auto entities = chunk->entities;
            chunk->entityMutex.unlock();

            for (auto &entity : entities) {
                entity.second->update();
            }
        }
    }
}
