#pragma once

#include <vector>
#include <string>
#include "PlayerEvent.h"
#include <gameocean/plugin/event/CallbackList.h>
#include <gameocean/utils/Cancellable.h>

class EntityDamageEvent;

class PlayerDamageEvent : public PlayerEvent, public Cancellable {

public:
    static CallbackList<PlayerDamageEvent> callbacks;

protected:
    EntityDamageEvent &event;

public:
    PlayerDamageEvent(Player &player, EntityDamageEvent &event) : PlayerEvent(player), event(event) {
        //
    }

    inline EntityDamageEvent &getEvent() {
        return event;
    }

};