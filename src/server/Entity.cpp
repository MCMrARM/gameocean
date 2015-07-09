#include "Entity.h"

#include "common.h"
#include "world/World.h"
#include "world/Chunk.h"

EntityId Entity::currentId = 0;

Entity::~Entity() {
    if (chunk != null) {
        chunk->removeEntity(this);
    }
}

void Entity::setPos(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;

    int newChunkX = ((int) x) >> 4;
    int newChunkZ = ((int) z) >> 4;
    if (chunk == null) {
        chunk = world.getChunkAt(newChunkX, newChunkZ);
        chunk->addEntity(this);
    } else if (newChunkX != chunk->pos.x || newChunkZ != chunk->pos.z) {
        chunk->removeEntity(this);
        chunk = world.getChunkAt(newChunkX, newChunkZ, true);
        chunk->addEntity(this);
    }
}