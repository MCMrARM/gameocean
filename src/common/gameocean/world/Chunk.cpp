#include "Chunk.h"

#include "tile/Tile.h"

Chunk* Chunk::empty = new Chunk(0, 0);

std::shared_ptr<Tile> Chunk::getTile(int x, int y, int z) {
    BlockPos p = {x, y, z};
    for (std::shared_ptr<Tile> const& t : tiles) {
        if (t->getPos() == p)
            return t;
    }
    return nullptr;
}