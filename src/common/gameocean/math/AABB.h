#pragma once


class AABB {
public:
    float minX, minY, minZ, maxX, maxY, maxZ;

    AABB const& translate(float x, float y, float z) {
        this->minX += x;
        this->maxX += x;
        this->minY += y;
        this->maxY += y;
        this->minZ += z;
        this->maxZ += z;
        return *this;
    }

    AABB const& expand(float x, float y, float z) {
        this->minX -= x;
        this->maxX += x;
        this->minY -= y;
        this->maxY += y;
        this->minZ -= z;
        this->maxZ += z;
        return *this;
    }

    AABB const& add(float x, float y, float z) {
        if (x > 0)
            this->maxX += x;
        else
            this->minX += x;

        if (y > 0)
            this->maxY += y;
        else
            this->minY += y;

        if (z > 0)
            this->maxZ += z;
        else
            this->minZ += z;
        return *this;
    }


    bool intersects(AABB const& aabb) const {
        return (aabb.minX <= maxX && aabb.maxX >= minX &&
                aabb.minY <= maxY && aabb.maxY >= minY &&
                aabb.minZ <= maxZ && aabb.maxZ >= minZ);
    }
};


