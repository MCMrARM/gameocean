#pragma once

#include <gameocean/plugin/event/Event.h>
class Player;

class PlayerEvent : public Event {

protected:
    Player& player;

public:
    PlayerEvent(Player &player) : player(player) {
        //
    }

    inline Player &getPlayer() {
        return player;
    }

};