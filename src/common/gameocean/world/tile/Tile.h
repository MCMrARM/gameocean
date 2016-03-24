#pragma once

#include <memory>
#include <map>
class World;
#include "../BlockPos.h"

class Tile {

private:
    World& world;
    BlockPos pos;

public:
    typedef std::shared_ptr<Tile> (*TileCreator)(World& world, BlockPos pos);
    static std::map<std::string, TileCreator> tiles;
    template<typename T>
    static std::shared_ptr<Tile> createTile(World& world, BlockPos pos) {
        return std::shared_ptr<Tile>(new T(world, pos));
    }
    static std::shared_ptr<Tile> createTile(std::string tile, World& world, BlockPos pos) {
        if (tiles.count(tile) <= 0)
            return nullptr;
        return tiles[tile](world, pos);
    }
    template<typename T>
    static void registerTile() {
        tiles[T::name] = &createTile<T>;
    }
    static void registerTiles();

    Tile(World& world, BlockPos pos) : world(world), pos(pos) {
        //
    }

    virtual const char* getId() = 0;

    inline World& getWorld() {
        return world;
    }
    inline BlockPos const& getPos() {
        return pos;
    }

};


