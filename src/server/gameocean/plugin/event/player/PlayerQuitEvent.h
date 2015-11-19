#pragma once

#include <vector>
#include <string>
#include "PlayerEvent.h"
#include "../CallbackList.h"
#include <gameocean/utils/Cancellable.h>

class PlayerQuitEvent : public PlayerEvent, public Cancellable {

public:
    static CallbackList<ChatEvent> callbacks;

protected:
    std::string reason;

public:
    PlayerQuitEvent(Player& player, std::string const& reason) : PlayerEvent(player), reason(reason) {
        //
    }

    inline std::string& getReason() {
        return reason;
    }

};