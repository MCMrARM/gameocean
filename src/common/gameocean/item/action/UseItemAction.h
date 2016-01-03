#pragma once

#include <map>
#include "ItemAction.h"
#include "../../world/BlockPos.h"
class World;
class BlockVariant;
class Player;

class UseItemAction : public ItemAction {

public:
    typedef bool (*Handler)(UseItemAction&);
    static std::map<std::string, Handler> handlers;

protected:
    Player* player;
    World& world;
    BlockVariant* block;
    BlockPos pos;
    BlockPos::Side side;

public:
    UseItemAction(Player* player, ItemVariant* item, World& world, BlockVariant* block, BlockPos pos,
                  BlockPos::Side side) : ItemAction(item), player(player), world(world), block(block), pos(pos), side(side) {
        //
    }

    inline BlockVariant* getTargetBlockVariant() {
        return block;
    }

    inline Player* getPlayer() {
        return player;
    }

    inline World& getWorld() {
        return world;
    }

    inline BlockPos const& getTargetBlockPos() {
        return pos;
    }

    inline BlockPos::Side getTargetBlockSide() {
        return side;
    }

    inline bool isUsedOnAir() {
        return (block == nullptr);
    }

};