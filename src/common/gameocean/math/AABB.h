#pragma once


class AABB {
public:
    float minX, minY, minZ, maxX, maxY, maxZ;

    AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) : minX(minX), minY(minY), minZ(minZ),
                                                                                   maxX(maxX), maxY(maxY), maxZ(maxZ) { }
    AABB(AABB const& c) : minX(c.minX), minY(c.minY), minZ(c.minZ), maxX(c.maxX), maxY(c.maxY), maxZ(c.maxZ) {}

    AABB const& translate(float x, float y, float z) {
        this->minX += x;
        this->maxX += x;
        this->minY += x;
        this->maxY += x;
        this->minZ += x;
        this->maxZ += x;
        return *this;
    }
};


