#pragma once

#include <string>
#include <map>
#include <set>
#include <gameocean/common.h>
#include <gameocean/item/action/UseItemAction.h>

class ItemGroup;
class BlockGroup;
class UseItemAction;

class ItemVariant {
protected:
    int id;
    short variantDataId;
    std::string stringId;

    int maxStackSize = 64;

public:
    ItemGroup* toolGroup = nullptr;
    std::set<BlockGroup*> toolAffects;
    float toolBreakMultiplier = 1.0f;

    UseItemAction::Handler useAction = nullptr;

    ItemVariant(int id, short variantDataId, std::string stringId);
    inline int getId() { return id; };
    virtual std::string const& getStringId() { return stringId; };
    inline short getVariantDataId() { return variantDataId; };
    virtual std::string getNameId() { return std::string("item.") + stringId + ".name"; };
    std::string getName();

    void copyItemProperties(ItemVariant const& item) {
        this->maxStackSize = item.maxStackSize;
        this->toolGroup = item.toolGroup;
        this->toolAffects = item.toolAffects;
        this->toolBreakMultiplier = item.toolBreakMultiplier;
    };

    inline int getMaxStackSize() { return maxStackSize; };
    inline void setMaxStackSize(int size) { maxStackSize = size; };

    virtual bool use(UseItemAction& action);
};
