#pragma once

#include "Container.h"

class Chest : public Container {

public:
    static const char *name;

    Chest(World &world, BlockPos pos) : Container(world, pos, 27) {
    }

    virtual const char *getId() {
        return name;
    }

};


