#include "InventoryTransaction.h"

#include <set>
#include "../../Player.h"

bool InventoryTransaction::isFinished() {
    if (!isStillValid() || elements.size() < 2)
        return false;

    if (isInInventoryTransfer())
        return false;
    if (isContainerTransfer())
        return true;
    return false;
}

bool InventoryTransaction::isValid() {
    if (!isStillValid() || elements.size() < 2)
        return false;

    if (isInInventoryTransfer())
        return false; // no support yet too
    if (isContainerTransfer()) {
        int totalPlayerDiff = 0;
        int totalContainerDiff = 0;
        ItemInstance* i = &elements[0].toItem;
        if (i->isEmpty()) {
            i = &elements[0].fromItem;
            if (i->isEmpty())
                return false;
        }
        for (Element e : elements) {
            if (!e.toItem.isSameType(*i) && !e.toItem.isEmpty())
                return false;
            if (!e.fromItem.isSameType(e.toItem) && !e.fromItem.isEmpty() && !e.toItem.isEmpty())
                return false;
            if (&e.inventory == &owner.inventory) {
                totalPlayerDiff += e.getDiff();
            } else {
                totalContainerDiff += e.getDiff();
            }
        }
        return (totalPlayerDiff == -totalContainerDiff);
    }
    return false;
}

bool InventoryTransaction::isStillValid() {
    for (Element e : elements) {
        if (e.inventory.getItem(e.slot) != e.fromItem)
            return false;
    }
    return true;
}

bool InventoryTransaction::isInInventoryTransfer() {
    for (Element e : elements) {
        if (&e.inventory != &owner.inventory) {
            return false;
        }
    }
    return true;
}

bool InventoryTransaction::isContainerTransfer() {
    bool hasPlayerInventory = false;
    Inventory* secondInventory = nullptr;
    for (Element e : elements) {
        if (&e.inventory == &owner.inventory) {
            hasPlayerInventory = true;
        } else {
            if (secondInventory == nullptr)
                secondInventory = &e.inventory;
            else if (secondInventory != &e.inventory)
                return false;
        }
    }
    return (hasPlayerInventory && secondInventory != nullptr);
}

void InventoryTransaction::execute() {
    for (Element e : elements) {
        e.inventory.setItem(e.slot, e.toItem);
    }
}

void InventoryTransaction::revert() {
    bool hasPlayerInventory = false;
    bool hasSecondInventory = false;
    for (Element e : elements) {
        if (&e.inventory == &owner.inventory) {
            hasPlayerInventory = true;
        } else {
            hasSecondInventory = true;
        }
    }
    if (hasPlayerInventory)
        owner.sendInventory();
    if (hasSecondInventory)
        owner.sendContainerContents();
}

void InventoryTransaction::reset() {
    elements.clear();
}