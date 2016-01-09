#include "InventoryTransaction.h"

#include <set>
#include "../../Player.h"

bool InventoryTransaction::isFinished() {
    if (!isStillValid() || elements.size() < 2)
        return false;

    if (isArmorSlotTransfer())
        return true;
    if (isInInventoryTransfer())
        return false;
    if (isContainerTransfer())
        return true;
    return false;
}

bool InventoryTransaction::isValid() {
    if (!isStillValid() || elements.size() < 2)
        return false;

    if (isArmorSlotTransfer()) {
        Element const& e1 = elements[0];
        Element const& e2 = elements[1];
        return (e1.toItem.isSameType(e2.fromItem) && e1.fromItem.isSameType(e2.toItem) && e1.getDiff() == -e2.getDiff());
    }
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
        if (e.kind == InventoryKind::ARMOR) {
            if (((InventoryWithArmor&) e.inventory).getArmorSlot(e.slot) != e.fromItem)
                return false;
            continue;
        }
        else if (e.inventory.getItem(e.slot) != e.fromItem)
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
    for (Element const& e : elements) {
        if (&e.inventory == &owner.inventory && e.kind == InventoryKind::PLAYER) {
            hasPlayerInventory = true;
        } else if (&e.inventory != &owner.inventory && e.kind == InventoryKind::CONTAINER) {
            if (secondInventory == nullptr)
                secondInventory = &e.inventory;
            else if (secondInventory != &e.inventory)
                return false;
        }
    }
    return (hasPlayerInventory && secondInventory != nullptr);
}

bool InventoryTransaction::isArmorSlotTransfer() {
    if (elements.size() != 2)
        return false;
    Element const& e1 = elements[0];
    Element const& e2 = elements[1];
    return (&e1.inventory == &e2.inventory &&
            ((e1.kind == InventoryKind::PLAYER && e2.kind == InventoryKind::ARMOR)
             || (e2.kind == InventoryKind::PLAYER && e1.kind == InventoryKind::ARMOR)));
}

void InventoryTransaction::execute() {
    for (Element e : elements) {
        if (e.kind == InventoryKind::ARMOR) {
            ((InventoryWithArmor&) e.inventory).setArmorSlot(e.slot, e.toItem);
            continue;
        }
        e.inventory.setItem(e.slot, e.toItem);
    }
}

void InventoryTransaction::revert() {
    bool hasPlayerInventory = false;
    bool hasArmorInventory = false;
    bool hasSecondInventory = false;
    for (Element e : elements) {
        if (&e.inventory == &owner.inventory) {
            if (e.kind == InventoryKind::PLAYER) {
                hasPlayerInventory = true;
            } else if (e.kind == InventoryKind::ARMOR) {
                hasArmorInventory = true;
            }
        } else if (e.kind == InventoryKind::CONTAINER) {
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