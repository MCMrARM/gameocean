#pragma once

#include "Tile.h"
#include <gameocean/game/Inventory.h>

class Chest : public Tile {

protected:
    Inventory inventory;

public:
    static std::string name;

    Chest(World& world, BlockPos pos) : Tile(world, pos), inventory(27) {
        //
    }

    virtual std::string getId() {
        return name;
    }

    inline Inventory& getInventory() {
        return inventory;
    }

};


