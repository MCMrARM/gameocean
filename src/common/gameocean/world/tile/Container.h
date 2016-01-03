#pragma once

#include "Tile.h"
#include <gameocean/game/Inventory.h>

class Container : public Tile {

protected:
    Inventory inventory;

public:
    Container(World& world, BlockPos pos, int slots) : Tile(world, pos), inventory(slots) {
        //
    }

    inline Inventory& getInventory() {
        return inventory;
    }

};


