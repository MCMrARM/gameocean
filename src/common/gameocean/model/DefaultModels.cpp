#include "DefaultModels.h"
#include "../world/World.h"

void DefaultModels::registerModels() {
    DynamicModel::models["dynamic:door"] = new DoorDynamicModel();
}

DoorDynamicModel::DoorDynamicModel() {
    doorEast = Model::getModel("door_east");
    doorSouth = Model::getModel("door_south");
    doorWest = Model::getModel("door_west");
    doorNorth = Model::getModel("door_north");
}

std::vector<AABB> &DoorDynamicModel::getAABBs(World &world, BlockPos pos, BlockVariant *variant) {
    byte data = world.getBlock(pos).data;
    byte dataTop = 0, dataBottom = 0;
    if ((data & 8) != 0) { // top
        dataTop = data;
        WorldBlock b = world.getBlock(pos.side(BlockPos::Side::DOWN));
        if (b.getBlockVariant() == variant)
            dataBottom = b.data;
    } else { // bottom
        dataBottom = data;
        WorldBlock b = world.getBlock(pos.side(BlockPos::Side::UP));
        if (b.getBlockVariant() == variant)
            dataTop = b.data;
    }
    int facing = dataBottom & 3;
    if ((dataBottom & 4) != 0) { // is opened
        if (dataTop & 1) { // opens right
            facing--;
        } else {
            facing++;
        }
        if (facing == 5)
            facing = 0;
        if (facing == -1)
            facing = 3;
    }
    switch (facing) {
        case 0:
            return doorEast->aabbs;
        case 1:
            return doorSouth->aabbs;
        case 2:
            return doorWest->aabbs;
        case 3:
            return doorNorth->aabbs;
        default:
            return doorEast->aabbs;
    }
}