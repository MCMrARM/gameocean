#include "ItemInstance.h"
#include "ItemRegister.h"
#include "ItemVariant.h"

void ItemInstance::setItem(int id, short data) {
    item = ItemRegister::getItemVariant(id, data);
    this->data = data;
    nameOverride = "";
}

int ItemInstance::getItemId() {
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

void ItemInstance::setItemDamage(short data) {
    item = ItemRegister::getItemVariant(item->getId(), data);
    this->data = data;
}