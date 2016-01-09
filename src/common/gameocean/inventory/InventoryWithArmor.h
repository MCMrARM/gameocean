#pragma once

#include "Inventory.h"

class InventoryWithArmor : public Inventory {

protected:
    ItemInstance armor[4];

public:
    InventoryWithArmor(int numSlots) : Inventory(numSlots) {
        //
    };

    inline ItemInstance getArmorSlot(int slot) {
        std::unique_lock<std::mutex> lock (mutex);
        return armor[slot];
    };
    virtual void setArmorSlot(int slot, ItemInstance item) {
        std::unique_lock<std::mutex> lock (mutex);
        armor[slot] = item;
    };

};