#pragma once

struct ChunkPos {

    int x, z;

    bool operator<(const ChunkPos &o) const {
        return (o.x < x || (o.x == 0 && o.z < z));
    };
    bool operator=(const ChunkPos &o) const {
        return (o.x == x && o.z == z);
    };

};