#pragma once

struct BlockPos {

    int x, y, z;

    bool operator==(const BlockPos &o) const {
        return (o.x == x && o.y == y && o.z == z);
    };

};