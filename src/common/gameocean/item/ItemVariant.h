#pragma once

#include <string>
#include <map>
#include <set>
#include <memory>
#include <gameocean/common.h>
#include "action/UseItemAction.h"
#include "action/ActionHandlerData.h"

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
    ItemGroup *toolGroup = nullptr;
    std::set<BlockGroup*> toolAffects;
    float toolBreakMultiplier = 1.0f;
    float attackDamage = 1.f;
    float damageReduction = 0.f;
    int maxDamage = 0;
    int itemDamageBlockBreak = 0;
    int itemDamageAttack = 0;
    bool isFood = false;
    float restoreFoodPoints = 0.f;
    float restoreFoodSaturation = 0.f;

    UseItemAction::Handler useAction = nullptr;
    std::unique_ptr<ActionHandlerData> useActionData;

    ItemVariant(int id, short variantDataId, std::string stringId);
    inline int getId() { return id; };
    virtual std::string const &getStringId() { return stringId; };
    inline short getVariantDataId() { return variantDataId; };
    virtual std::string getNameId() { return std::string("item.") + stringId + ".name"; };
    std::string getName();

    void copyItemProperties(ItemVariant const &item) {
        this->maxStackSize = item.maxStackSize;
        this->toolGroup = item.toolGroup;
        this->toolAffects = item.toolAffects;
        this->toolBreakMultiplier = item.toolBreakMultiplier;
        this->attackDamage = item.attackDamage;
        this->damageReduction = item.damageReduction;
        this->maxDamage = item.maxDamage;
        this->itemDamageBlockBreak = item.itemDamageBlockBreak;
        this->itemDamageAttack = item.itemDamageAttack;
        this->isFood = item.isFood;
        this->restoreFoodPoints = item.restoreFoodPoints;
        this->restoreFoodSaturation = item.restoreFoodSaturation;
    }

    inline int getMaxStackSize() { return maxStackSize; }
    inline void setMaxStackSize(int size) { maxStackSize = size; }

    virtual bool use(UseItemAction &action);
};
