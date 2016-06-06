#pragma once

#include <map>
#include "BlockAction.h"

class DestroyBlockAction : public BlockAction {

public:
    typedef bool (*Handler)(DestroyBlockAction &, ActionHandlerData *);
    static std::map<std::string, Handler> handlers;
    static std::map<std::string, ItemAction::ProcessDataHandler> processHandlers;

    DestroyBlockAction(BlockVariant *block, BlockPos pos) : BlockAction(block, pos) {
        //
    }

};