#include "Inventory.h"

#include <gameocean/game/item/ItemVariant.h>

bool Inventory::addItem(ItemInstance item) {
    for (int i = 0; i < items.size(); i++) {
        ItemInstance s = items[i];
        if (s.isSameType(item) && s.getItem()->getMaxStackSize() > s.count) {
            if (s.count + item.count > s.getItem()->getMaxStackSize()) {
                item.count -= s.getItem()->getMaxStackSize() - s.count;
                s.count = s.getItem()->getMaxStackSize();
                setItem(i, s);
            } else {
                s.count += item.count;
                setItem(i, s);
                return true;
            }
        }
    }

    for (int i = 0; i < items.size(); i++) {
        ItemInstance s = items[i];
        if (s.isEmpty()) {
            s.setItem(item.getItem(), item.getItemData());
            if (item.count > s.getItem()->getMaxStackSize()) {
                s.count = s.getItem()->getMaxStackSize();
                item.count -= s.count;
                setItem(i, s);
            } else {
                s.count = item.count;
                setItem(i, s);
                return true;
            }
        }
    }
    return false;
}

bool Inventory::removeItem(ItemInstance item) {
    for (int i = 0; i < items.size(); i++) {
        ItemInstance s = items[i];
        if (s.isSameType(item)) {
            if (s.count - item.count < 0) {
                item.count -= s.count;
                setItem(i, ItemInstance ());
            } else {
                s.count -= item.count;
                if (s.count == 0) {
                    s.setEmpty();
                }
                setItem(i, s);
                return true;
            }
        }
    }

    return false;
}

bool Inventory::findItem(ItemInstance const& item, bool exact) {
    int c = item.count;
    for (ItemInstance& s : items) {
        if (s.isSameType(item)) {
            if (exact) {
                if (s.count == item.count)
                    return true;
            } else {
                c -= s.count;
                if (c <= 0)
                    return true;
            }
        }
    }
    return false;
}