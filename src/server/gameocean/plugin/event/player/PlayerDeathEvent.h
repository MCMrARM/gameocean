#pragma once

#include <vector>
#include <string>
#include "PlayerEvent.h"
#include "../CallbackList.h"
#include <gameocean/utils/Cancellable.h>
#include <gameocean/utils/Vector3D.h>

class World;

class PlayerDeathEvent : public PlayerEvent, public Cancellable {

protected:
    World* world;
    Vector3D pos;

public:
    static CallbackList<PlayerDeathEvent> callbacks;

    PlayerDeathEvent(Player& player, World* world, Vector3D pos) : PlayerEvent(player), world(world), pos(pos) {
        //
    }

    inline World* getWorld() {
        return world;
    }

    inline Vector3D& getRespawnPos() {
        return pos;
    }

    inline void setWorld(World* world) {
        this->world = world;
    }

    inline void setRespawnPos(Vector3D pos) {
        this->pos = pos;
    }

};