#pragma once

#include "../Event.h"
class Entity;

class EntityEvent : public Event {

protected:
    Entity& entity;

public:
    EntityEvent(Entity& entity) : entity(entity) {
        //
    }

    inline Entity& getEntity() {
        return entity;
    }

};