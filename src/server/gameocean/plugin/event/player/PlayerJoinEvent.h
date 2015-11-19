#pragma once

#include <vector>
#include <string>
#include "PlayerEvent.h"
#include "../CallbackList.h"
#include <gameocean/utils/Cancellable.h>

class PlayerJoinEvent : public PlayerEvent, public Cancellable {

public:
    static CallbackList<ChatEvent> callbacks;

    PlayerJoinEvent(Player& player) : PlayerEvent(player) {
        //
    }

};