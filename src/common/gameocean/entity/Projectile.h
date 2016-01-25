#pragma once

#include "Entity.h"

class Projectile : public Entity {

protected:
    Entity* thrownBy;

public:
    Projectile(World& world, Entity* thrownBy) : Entity(world), thrownBy(thrownBy) {
        gravity = 0.03f;
        drag = 0.01f;
    }

    inline Entity* getThrownBy() {
        return thrownBy;
    }

};


