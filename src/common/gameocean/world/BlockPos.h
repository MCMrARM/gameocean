#pragma once

#include <iostream>

struct BlockPos {

    enum class Side {
        DOWN, UP, NORTH, SOUTH, WEST, EAST
    };

    int x, y, z;

    bool operator==(const BlockPos &o) const {
        return (o.x == x && o.y == y && o.z == z);
    };

    BlockPos side(Side side) const {
        if (side == Side::DOWN)
            return { x, y - 1, z };
        if (side == Side::UP)
            return {x, y + 1, z};
        if (side == Side::NORTH)
            return { x, y, z - 1 };
        if (side == Side::SOUTH)
            return { x, y, z + 1 };
        if (side == Side::WEST)
            return { x - 1, y, z };
        if (side == Side::EAST)
            return { x + 1, y, z };

        return { x, y, z };
    };

};