#pragma once

#include <map>
#include <memory>
#include "ItemAction.h"
#include "../../world/BlockPos.h"
#include "../../math/Vector3D.h"
class World;
class BlockVariant;
class Player;

class UseItemAction : public ItemAction {

public:
    typedef bool (*Handler)(UseItemAction&, ActionHandlerData*);
    static std::map<std::string, Handler> handlers;
    static std::map<std::string, ProcessDataHandler> processHandlers;

protected:
    std::shared_ptr<Player> player;
    World& world;
    BlockVariant* block;
    BlockPos pos;
    BlockPos::Side side;
    Vector3D touchVec;

public:
    UseItemAction(std::shared_ptr<Player> player, ItemVariant* item, World& world, BlockVariant* block, BlockPos pos,
                  BlockPos::Side side, Vector3D touchVec) : ItemAction(item), player(player), world(world), block(block),
                                                            pos(pos), side(side), touchVec(touchVec) {
        //
    }

    inline BlockVariant* getTargetBlockVariant() const {
        return block;
    }

    inline std::shared_ptr<Player> getPlayer() {
        return player;
    }

    inline World& getWorld() const {
        return world;
    }

    inline BlockPos const& getTargetBlockPos() const {
        return pos;
    }

    inline BlockPos::Side getTargetBlockSide() const {
        return side;
    }

    inline bool isUsedOnAir() const {
        return (block == nullptr);
    }

    inline Vector3D const& getTouchVector() const {
        return touchVec;
    }

};