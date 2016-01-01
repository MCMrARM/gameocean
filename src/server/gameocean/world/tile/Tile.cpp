#include "Tile.h"
#include "Chest.h"

std::map<std::string, Tile::TileCreator> Tile::tiles;

void Tile::registerTiles() {
    registerTile<Chest>();
}