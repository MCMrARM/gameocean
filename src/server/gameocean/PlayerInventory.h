#pragma once

#include <gameocean/inventory/InventoryWithArmor.h>
class Player;

class PlayerInventory : public InventoryWithArmor {

public:
    Player &player;
    int heldSlot = -1;

    PlayerInventory(Player &player, int numSlots) : InventoryWithArmor(numSlots), player(player) { };

    virtual void setItem(int slot, ItemInstance item);

    virtual void setArmorSlot(int slot, ItemInstance item);

    inline int getHeldSlot() {
        mutex.lock();
        int ret = heldSlot;
        mutex.unlock();
        return ret;
    };
    void setHeldSlot(int v);
    inline ItemInstance getHeldItem() {
        return getItem(getHeldSlot());
    };

};


