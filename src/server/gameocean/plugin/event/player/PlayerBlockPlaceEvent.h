#pragma once

#include <vector>
#include <string>
#include "PlayerEvent.h"
#include <gameocean/plugin/event/CallbackList.h>
#include <gameocean/utils/Cancellable.h>
#include <gameocean/world/BlockPos.h>

class BlockVariant;

class PlayerBlockPlaceEvent : public PlayerEvent, public Cancellable {

public:
    static CallbackList<PlayerBlockPlaceEvent> callbacks;

protected:
    World &world;
    BlockVariant *block;
    BlockPos pos;
    BlockPos::Side side;

public:
    PlayerBlockPlaceEvent(Player& player, World &world, BlockVariant *block, BlockPos pos, BlockPos::Side side) :
            PlayerEvent(player), world(world), block(block), pos(pos), side(side) {
        //
    }

    inline World &getWorld() {
        return world;
    }

    inline BlockVariant *getBlockVariant() {
        return block;
    }

    inline BlockPos const &getPosition() {
        return pos;
    }

    inline BlockPos::Side getSide() {
        return side;
    }

};