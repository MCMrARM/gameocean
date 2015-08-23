#pragma once

#include <string>
#include <map>
#include "common.h"

class Item {
protected:
    int id;
    std::string stringId;

    int maxStackSize = 64;

    static Item* itemsByIds[512];
    static std::map<std::string, Item*> items;

public:
    static inline Item* getItem(int id) { return itemsByIds[id]; };
    static inline Item* getItem(std::string id) {
        if (items.count(id) > 0)
            return items.at(id);
        return null;
    };
    static void registerItems();

    Item(int id, std::string stringId);
    inline int getId() { return id; };
    virtual std::string getNameId() { return std::string("item.") + stringId + ".name"; };
    std::string getName();

    inline int getMaxStackSize() { return maxStackSize; };
    inline void setMaxStackSize(int size) { maxStackSize = size; };
};
