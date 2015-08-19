#pragma once

#include "common.h"

class Item;
class Block;

class ItemInstance {
protected:
    Item* item = null;
    std::string nameOverride;
public:
    byte count = 1;
    short damage = 0;

    ItemInstance() {};
    ItemInstance(Item* item) : item(item) {};
    ItemInstance(int id) { setId(id); };
    ItemInstance(Item* item, byte count, short damage) : item(item), count(count), damage(damage) {};
    ItemInstance(int id, byte count, short damage) : count(count), damage(damage) { setId(id); };

    void setId(int id);
    int getId();
    inline std::string& getNameOverride() { return nameOverride; };
    inline void overrideName(std::string val) { nameOverride = val; };
    std::string& getName();

};