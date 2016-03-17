#include "Snowball.h"
#include "../plugin/event/entity/EntityDamageEvent.h"

const char* Snowball::TYPE_NAME = "Snowball";

void Snowball::update() {
    Entity::update();
    {
        std::unique_lock<std::recursive_mutex> mutex (generalMutex);
        if (onGround) {
            kill();
            return;
        }
    }
    float existenceTime = getExistenceTime();
    for (std::shared_ptr<Entity>& ent : getNearbyEntities(0.3f)) {
        if (ent->isLiving() && (ent != thrownBy || existenceTime > 0.25f)) {
            EntityDamageEvent event (*ent, 0.f, EntityDamageEvent::DamageSource::ENTITY, this, 0.3f);
            ent->damage(event);
            if (!event.isCancelled()) {
                kill();
                break;
            }
        }
    }
}