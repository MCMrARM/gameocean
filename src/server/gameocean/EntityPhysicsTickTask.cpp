#include "EntityPhysicsTickTask.h"

#include "Server.h"
#include <gameocean/world/World.h>

void EntityPhysicsTickTask::run() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(server.physicsTickRate));

        if (shouldStop)
            break;

        World* world = server.mainWorld;
        for (auto const& it : world->getChunks()) {
            Chunk* chunk = it.second;
            chunk->mutex.lock();
            for (auto const& ent : chunk->entities) {
                Entity* entity = ent.second;
                entity->tickPhysics();
            }
            chunk->mutex.unlock();
        }
    }
}
