#pragma once

#include <map>
#include <string>
#include <vector>
#include "../math/AABB.h"
#include "../world/BlockPos.h"
class World;
class BlockVariant;

/**
 * Dynamic Models are models that are implemented in code and can vary, depending on situation.
 */
class DynamicModel {

public:
    static std::map<std::string, DynamicModel *> models;

    virtual std::vector<AABB> &getAABBs(World &world, BlockPos pos, BlockVariant *variant) = 0;

};

