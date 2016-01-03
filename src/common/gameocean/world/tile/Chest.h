#pragma once

#include "Container.h"

class Chest : public Container {

public:
    static std::string name;

    Chest(World& world, BlockPos pos) : Container(world, pos, 27) {
    }

    virtual std::string getId() {
        return name;
    }

};


