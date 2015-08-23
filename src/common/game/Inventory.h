#pragma once

#include <vector>
#include <mutex>
#include "ItemInstance.h"

class Inventory {

public:
    const int numSlots;
    std::vector<ItemInstance> items;
    std::mutex mutex;

    Inventory(int numSlots) : numSlots(numSlots) {
        items.resize(numSlots);
    };

    inline int getNumSlots() { return numSlots; };

    virtual void setItem(int slot, ItemInstance item) {
        mutex.lock();
        items[slot] = item;
        mutex.unlock();
    };
    bool addItem(ItemInstance item);
    bool removeItem(ItemInstance item);
    bool findItem(ItemInstance item, bool exact);

    inline ItemInstance getItem(int slot) {
        if (slot < 0 || slot >= numSlots)
            return ItemInstance ();

        mutex.lock();
        ItemInstance ret = items[slot];
        mutex.unlock();
        return ret;
    };

};


