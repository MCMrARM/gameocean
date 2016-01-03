#pragma once

#include <gameocean/common.h>
#include <gameocean/item/BlockVariant.h>
#include <gameocean/item/ItemRegister.h>

struct WorldBlock {
    BlockId id;
    byte data;

    inline BlockVariant* getBlockVariant() {
        return ItemRegister::getBlockVariant(id, data);
    }
};