#include <utils/StringUtils.h>
#include "Item.h"

std::map<std::string, Item*> Item::items = std::map<std::string, Item*>();
Item* Item::itemsByIds [512];

void Item::registerItems() {
    //
}

Item* Item::findItem(std::string id) {
    int iid = -1;
    try {
        iid = std::stoi(id);
    } catch (std::exception e) {
    }
    if (iid > 0 && iid < 512) {
        Item* ret = Item::getItem(iid);
        if (ret != null)
            return ret;
    }

    int m = 0;
    Item* ret = null;
    for (auto const& e : Item::items) {
        int i = StringUtils::compare(e.first, id);
        if (i > m) {
            i = m;
            ret = e.second;
        }
    }
    return ret;
}

Item::Item(int id, std::string stringId) : id(id), stringId(stringId) {
    items[stringId] = this;
    itemsByIds[id] = this;
}

std::string Item::getName() {
    return getNameId();
}