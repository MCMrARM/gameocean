#pragma once

#include <map>
#include "BlockAction.h"
class Player;

class DestroyBlockAction : public BlockAction {

protected:
    std::shared_ptr<Player> player;

public:
    typedef bool (*Handler)(DestroyBlockAction &, ActionHandlerData *);
    static std::map<std::string, Handler> handlers;
    static std::map<std::string, ItemAction::ProcessDataHandler> processHandlers;

    DestroyBlockAction(World &world, BlockVariant *block, BlockPos pos, std::shared_ptr<Player> player) :
            BlockAction(world, block, pos), player(player) {
        //
    }

    inline std::shared_ptr<Player> getPlayer() {
        return player;
    }

};