#pragma once

#include <string>
#include "ItemInstance.h"

class ItemVariant;
class ItemGroup;

struct ItemDrop {
    std::string dropVariantId;
    int dropCount = 1;
    ItemGroup* requiredGroup = nullptr;
    std::string requiredVariantId;
    float chances = 1.f;
};