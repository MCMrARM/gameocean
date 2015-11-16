#pragma once

#include <functional>

struct ChunkPos {

    int x, z;

    ChunkPos() {};
    ChunkPos(int x, int z) : x(x), z(z) { };
    ChunkPos(const ChunkPos& pos) : x(pos.x), z(pos.z) { };

    bool operator<(const ChunkPos &o) const {
        return (o.x < x || (o.x == x && o.z < z));
    };
    bool operator==(const ChunkPos &o) const {
        return (o.x == x && o.z == z);
    };

};

namespace std {
    template <>
    struct hash<ChunkPos>
    {
        std::size_t operator()(const ChunkPos& k) const
        {
            return ((long long) k.x << 32) | k.z;
        }
    };
}