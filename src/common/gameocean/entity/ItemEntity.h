#pragma once

#include "Entity.h"
#include "../item/ItemInstance.h"

class ItemEntity : public Entity {

protected:
    ItemInstance itm;

public:
    ItemEntity(World& world, ItemInstance itm) : Entity(world), itm(itm) {
        sizeX = 0.25f;
        sizeY = 0.25f;
        headY = 0.125f;
        gravity = 0.04f;
    }

    static const char* TYPE_NAME;
    virtual const char* getTypeName() { return TYPE_NAME; };

    inline ItemInstance& getItem() {
        return itm;
    }

};


