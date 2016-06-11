#pragma once

#include "DynamicModel.h"

class Model;

class DefaultModels {

public:
    static void registerModels();

};

struct DoorDynamicModel : public DynamicModel {

    Model *doorEast, *doorSouth, *doorWest, *doorNorth;

    DoorDynamicModel();

    virtual std::vector<AABB> &getAABBs(World &world, BlockPos pos, BlockVariant *variant);

};

