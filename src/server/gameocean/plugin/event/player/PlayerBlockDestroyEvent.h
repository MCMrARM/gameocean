#pragma once

#include <vector>
#include <string>
#include "PlayerEvent.h"
#include <gameocean/plugin/event/CallbackList.h>
#include <gameocean/utils/Cancellable.h>
#include <gameocean/world/BlockPos.h>

class BlockVariant;

class PlayerBlockDestroyEvent : public PlayerEvent, public Cancellable {

public:
    static CallbackList<PlayerBlockDestroyEvent> callbacks;

protected:
    World &world;
    BlockVariant *block;
    BlockPos pos;

public:
    PlayerBlockDestroyEvent(Player &player, World &world, BlockVariant *block, BlockPos pos) :
            PlayerEvent(player), world(world), block(block), pos(pos) {
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

};