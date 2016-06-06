#pragma once

#include <vector>
#include <string>
#include "PlayerEvent.h"
#include <gameocean/plugin/event/CallbackList.h>
#include <gameocean/utils/Cancellable.h>

class PlayerJoinEvent : public PlayerEvent, public Cancellable {

public:
    static CallbackList<PlayerJoinEvent> callbacks;

    PlayerJoinEvent(Player &player) : PlayerEvent(player) {
        //
    }

};