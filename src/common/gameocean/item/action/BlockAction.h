#pragma once

#include "ItemAction.h"
#include "../../world/BlockPos.h"

class World;
class BlockVariant;

class BlockAction {

protected:
    World &world;
    BlockVariant *block;
    BlockPos pos;

public:
    BlockAction(World &world, BlockVariant *block, BlockPos pos) : world(world), block(block), pos(pos) {
        //
    }

    inline World &getWorld() {
        return world;
    }

    inline BlockVariant *getBlockVariant() {
        return block;
    }

    inline BlockPos const &getBlockPos() {
        return pos;
    }

};


