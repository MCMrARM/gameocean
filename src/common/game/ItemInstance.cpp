#include "ItemInstance.h"
#include "Item.h"

void ItemInstance::setId(int id) {
    item = Item::getItem(id);
    nameOverride = "";
}

int ItemInstance::getId() {
    if (item == null)
        return 0;
    return item->getId();
}

std::string ItemInstance::getName() {
    if (nameOverride.length() > 0) {
        return nameOverride;
    }
    if (item != null) {
        return item->getName();
    }
    return "empty";
}