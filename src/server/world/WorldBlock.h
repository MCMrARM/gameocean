#pragma once

#include "common.h"
#include "game/item/BlockVariant.h"
#include "game/item/ItemRegister.h"

struct WorldBlock {
    BlockId id;
    byte data;

    inline BlockVariant* getBlockVariant() {
        return ItemRegister::getBlockVariant(id, data);
    }
};