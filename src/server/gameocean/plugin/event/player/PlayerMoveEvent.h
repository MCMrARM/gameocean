#pragma once

#include <vector>
#include <string>
#include "PlayerEvent.h"
#include "../CallbackList.h"
#include <gameocean/utils/Cancellable.h>
#include <gameocean/utils/Vector3D.h>

class PlayerMoveEvent : public PlayerEvent, public Cancellable {

protected:
    Vector3D newPos;

public:
    static CallbackList<PlayerMoveEvent> callbacks;

    PlayerMoveEvent(Player& player, Vector3D newPos) : PlayerEvent(player), newPos(newPos) {
        //
    }

    inline Vector3D& getPos() {
        return newPos;
    }

};