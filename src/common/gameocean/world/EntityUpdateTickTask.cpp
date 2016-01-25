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
            std::unique_lock<std::recursive_mutex> lock (chunk->entityMutex);
            auto it2 = chunk->entities.begin();

            while (true) {
                if (it2 == chunk->entities.end())
                    break;
                Entity* entity = it2->second;
                it2++;
                entity->update();
            }
        }
    }
}
