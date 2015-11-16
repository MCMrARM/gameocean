#include "PlayerInventory.h"

#include "Player.h"

void PlayerInventory::setItem(int slot, ItemInstance item) {
    Inventory::setItem(slot, item);
    player.sendInventorySlot(slot);
}