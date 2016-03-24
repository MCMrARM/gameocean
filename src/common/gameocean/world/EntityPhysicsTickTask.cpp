#include "EntityPhysicsTickTask.h"

#include "World.h"

int EntityPhysicsTickTask::tickRate = 50;

void EntityPhysicsTickTask::run() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(EntityPhysicsTickTask::tickRate));

        if (shouldStop)
            break;

        for (auto& chunk : world.getChunkPtrs()) {
            chunk->entityMutex.lock();
            auto entities = chunk->entities;
            chunk->entityMutex.unlock();

            for (auto& entity : entities) {
                entity.second->tickPhysics();
            }
        }
    }
}
