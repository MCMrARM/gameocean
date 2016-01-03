#pragma once

#include <gameocean/inventory/Inventory.h>
class Player;

class PlayerInventory : public Inventory {

public:
    Player& player;
    int heldSlot = -1;

    PlayerInventory(Player& player, int numSlots) : Inventory(numSlots), player(player) { };

    virtual void setItem(int slot, ItemInstance item);

    inline int getHeldSlot() {
        mutex.lock();
        int ret = heldSlot;
        mutex.unlock();
        return ret;
    };
    inline void setHeldSlot(int v) {
        mutex.lock();
        heldSlot = v;
        mutex.unlock();
    };
    inline ItemInstance getHeldItem() {
        return getItem(getHeldSlot());
    };

};


