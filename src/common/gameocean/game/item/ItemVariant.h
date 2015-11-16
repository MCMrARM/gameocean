#pragma once

#include <string>
#include <map>
#include <set>
#include <gameocean/common.h>
class BlockGroup;

class ItemVariant {
protected:
    int id;
    short variantDataId;
    std::string stringId;

    int maxStackSize = 64;

public:
    std::set<BlockGroup*> toolAffects;
    float toolBreakMultiplier = 1.0f;

    ItemVariant(int id, short variantDataId, std::string stringId);
    inline int getId() { return id; };
    virtual std::string getStringId() { return stringId; };
    inline short getVariantDataId() { return variantDataId; };
    virtual std::string getNameId() { return std::string("item.") + stringId + ".name"; };
    std::string getName();

    void copyItemProperties(ItemVariant const& item) {
        this->maxStackSize = item.maxStackSize;
        this->toolAffects = item.toolAffects;
        this->toolBreakMultiplier = item.toolBreakMultiplier;
    };

    inline int getMaxStackSize() { return maxStackSize; };
    inline void setMaxStackSize(int size) { maxStackSize = size; };
};
