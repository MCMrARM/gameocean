#pragma once

#include <map>
#include "BlockAction.h"

class DestroyBlockAction : public BlockAction {

public:
    typedef bool (*Handler)(DestroyBlockAction&);
    static std::map<std::string, Handler> handlers;

    DestroyBlockAction(BlockVariant* block, BlockPos pos) : BlockAction(block, pos) {
        //
    }

};