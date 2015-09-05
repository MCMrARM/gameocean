#pragma once

#include <string>
#include <map>
#include "common.h"

class ItemVariant {
protected:
    int id;
    short variantDataId;
    std::string stringId;

    int maxStackSize = 64;

public:
    ItemVariant(int id, short variantDataId, std::string stringId);
    inline int getId() { return id; };
    virtual std::string getStringId() { return stringId; };
    inline short getVariantDataId() { return variantDataId; };
    virtual std::string getNameId() { return std::string("item.") + stringId + ".name"; };
    std::string getName();

    void copyItemProperties(ItemVariant const& item) {
        this->maxStackSize = item.maxStackSize;
    };

    inline int getMaxStackSize() { return maxStackSize; };
    inline void setMaxStackSize(int size) { maxStackSize = size; };
};
