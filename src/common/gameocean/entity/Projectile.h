#pragma once

#include "Entity.h"

class Projectile : public Entity {

protected:
    std::shared_ptr<Entity> thrownBy;

public:
    Projectile(World &world, std::shared_ptr<Entity> thrownBy) : Entity(world), thrownBy(thrownBy) {
        gravity = 0.03f;
        drag = 0.01f;
    }

    inline std::shared_ptr<Entity> getThrownBy() {
        return thrownBy;
    }

};


