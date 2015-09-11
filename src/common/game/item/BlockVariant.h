#pragma once

#include <string>
#include <map>
#include "common.h"

#include "ItemVariant.h"

typedef byte BlockId;
class BlockGroup;

class BlockVariant : public ItemVariant {
public:
    bool replaceable = false; // determines if the block can be replaced as it was air
    float hardness = 100;
    BlockGroup* blockGroup = null;
    bool needsTool = false;

    BlockVariant(int id, short variantDataId, std::string stringId);

    void copyBlockProperties(BlockVariant const& item) {
        this->replaceable = item.replaceable;
        this->hardness = item.hardness;
        this->blockGroup = item.blockGroup;
        this->needsTool = item.needsTool;
    };

    virtual std::string getNameId() { return std::string("block.") + stringId + ".name"; };

};


