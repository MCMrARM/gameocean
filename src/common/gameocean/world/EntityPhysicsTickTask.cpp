#include "EntityPhysicsTickTask.h"

#include "World.h"

int EntityPhysicsTickTask::tickRate = 50;

void EntityPhysicsTickTask::run() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(EntityPhysicsTickTask::tickRate));

        if (shouldStop)
            break;

        for (auto const& it : world.getChunks()) {
            Chunk* chunk = it.second;
            chunk->entityMutex.lock();
            for (auto const& ent : chunk->entities) {
                Entity* entity = ent.second;
                entity->tickPhysics();
            }
            chunk->entityMutex.unlock();
        }
    }
}
