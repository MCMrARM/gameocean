#include "Item.h"

std::map<std::string, Item*> Item::items = std::map<std::string, Item*>();
Item* Item::itemsByIds [512];

void Item::registerItems() {
    //
}

Item::Item(int id, std::string stringId) : id(id), stringId(stringId) {
    items[stringId] = this;
    itemsByIds[id] = this;
}

std::string Item::getName() {
    return getNameId();
}