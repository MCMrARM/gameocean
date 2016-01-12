#include "EntityUpdateTickTask.h"

#include "World.h"

int EntityUpdateTickTask::tickRate = 50;

void EntityUpdateTickTask::run() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(EntityUpdateTickTask::tickRate));

        if (shouldStop)
            break;

        for (auto const& it : world.getChunks()) {
            Chunk* chunk = it.second;
            chunk->entityMutex.lock();
            for (auto const& ent : chunk->entities) {
                Entity* entity = ent.second;
                entity->update();
            }
            chunk->entityMutex.unlock();
        }
    }
}
