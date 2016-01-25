#pragma once

#include "Projectile.h"

class Snowball : public Projectile {

public:
    Snowball(World& world, Entity* thrownBy = nullptr) : Projectile(world, thrownBy) {
        sizeX = 0.25f;
        sizeY = 0.25f;
    }

    static const char* TYPE_NAME;
    virtual const char* getTypeName() { return TYPE_NAME; };

    virtual void setPos(float x, float y, float z) {
        Entity::setPos(x, y, z);
    }

};


