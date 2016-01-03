#pragma once

#include "../../world/BlockPos.h"

class BlockVariant;

class BlockAction {

protected:
    BlockVariant* block;
    BlockPos pos;

public:
    BlockAction(BlockVariant* block, BlockPos pos) : block(block), pos(pos) {
        //
    }

    inline BlockVariant* getBlockVariant() {
        return block;
    }

    inline BlockPos const& getBlockPos() {
        return pos;
    }

};


