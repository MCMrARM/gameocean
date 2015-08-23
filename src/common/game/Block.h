#pragma once

#include <string>
#include <map>
#include "common.h"

#include "Item.h"

typedef byte BlockId;

class Block : public Item {
protected:
    static Block* blocksByIds[256];
    static std::map<std::string, Block*> blocks;

public:
    static inline Item* getBlock(int id) { return blocksByIds[id]; };
    static inline Item* getBlock(std::string id) {
        if (blocks.count(id) > 0)
            return blocks.at(id);
        return null;
    };
    static void registerBlocks();

    Block(int id, std::string stringId);

    virtual std::string getNameId() { return std::string("block.") + stringId + ".name"; };

};


