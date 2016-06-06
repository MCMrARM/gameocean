#pragma once

#include <vector>
#include <mutex>
#include <gameocean/item/ItemInstance.h>

class Inventory {

public:
    const int numSlots;
    std::vector<ItemInstance> items;
    std::mutex mutex;

    Inventory(int numSlots) : numSlots(numSlots) {
        items.resize((size_t) numSlots);
    }

    inline int getNumSlots() const { return numSlots; }

    virtual void setItem(int slot, ItemInstance item) {
        mutex.lock();
        items[slot] = item;
        mutex.unlock();
    }
    bool addItem(ItemInstance item);
    bool removeItem(ItemInstance item);
    bool findItem(ItemInstance const &item, bool exact);

    inline ItemInstance getItem(int slot) {
        if (slot < 0 || slot >= numSlots)
            return ItemInstance ();

        mutex.lock();
        ItemInstance ret = items[slot];
        mutex.unlock();
        return ret;
    }

    void clear() {
        mutex.lock();
        std::fill(items.begin(), items.end(), ItemInstance ());
        mutex.unlock();
    }

};


