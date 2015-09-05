#pragma once

#include <string>
#include <map>
#include "common.h"

#include "ItemVariant.h"

typedef byte BlockId;

class BlockVariant : public ItemVariant {
public:
    bool replaceable = false; // determines if the block can be replaced as it was air

    BlockVariant(int id, short variantDataId, std::string stringId);

    void copyBlockProperties(BlockVariant const& item) {
        this->replaceable = item.replaceable;
    };

    virtual std::string getNameId() { return std::string("block.") + stringId + ".name"; };

};


