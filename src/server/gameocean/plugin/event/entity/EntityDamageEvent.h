#pragma once
#pragma once

#include <vector>
#include <string>
#include "EntityEvent.h"
#include "../CallbackList.h"
#include <gameocean/utils/Cancellable.h>

class EntityDamageEvent : public EntityEvent, public Cancellable {

public:
    static CallbackList<EntityDamageEvent> callbacks;

    enum class DamageSource {
        UNKNOWN,
        SUICIDE,
        FALL,
        DROWNING,
        SUFFOCATION,
        ENTITY,
        BLOCK // for example cactus or lava
    };

protected:
    float hp;
    DamageSource source;
    Entity* attacker;
    float knockback;

public:
    EntityDamageEvent(Entity& entity, float hp, DamageSource source, Entity* attacker, float knockback) : EntityEvent(entity), hp(hp), attacker(attacker), knockback(knockback) {
        //
    }

    inline float getDamage() {
        return hp;
    }
    inline void setDamage(float hp) {
        this->hp = hp;
    }

    inline DamageSource getSource() {
        return source;
    }
    inline Entity* getAttacker() {
        return attacker;
    }

    inline float getKnockback() {
        return knockback;
    }
    inline void setKnockback(float knockback) {
        this->knockback = knockback;
    }

};