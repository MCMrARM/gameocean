#pragma once

#include <map>
#include "Chunk.h"
#include "ChunkPos.h"
#include "BlockPos.h"

class World {

private:
    std::string name;
    std::map<ChunkPos, Chunk*> chunks;

public:
    World(std::string name) : name(name) { };

    BlockPos spawn;

    inline std::string getName() { return name; };

    Chunk* getChunkAt(ChunkPos pos) {
        if (chunks.count(pos) > 0) {
            return chunks.at(pos);
        }
        return null;
    };
    inline Chunk* getChunkAt(int x, int z) {
        return getChunkAt({x, z});
    };

    void setChunk(Chunk* chunk) {
        Chunk* old = getChunkAt(chunk->pos);
        if (old != null) {
            delete old;
        }
        chunks[chunk->pos] = chunk;
    };

};


