#include "ItemInstance.h"
#include "ItemRegister.h"
#include "ItemVariant.h"

void ItemInstance::setItem(int id, short data) {
    item = ItemRegister::getItemVariant(id, data);
    this->data = data;
    nameOverride = "";
}

int ItemInstance::getItemId() const {
    if (item == nullptr)
        return 0;
    return item->getId();
}

std::string ItemInstance::getName() const {
    if (nameOverride.length() > 0) {
        return nameOverride;
    }
    if (item != nullptr) {
        return item->getName();
    }
    return "empty";
}

void ItemInstance::setItemDamage(short data) {
    item = ItemRegister::getItemVariant(item->getId(), data);
    this->data = data;
}

void ItemInstance::damageItem(int points) {
    setItemDamage(getItemData() + (short) points);
    if (item != nullptr && getItemData() > item->maxDamage) {
        setEmpty();
    }
}