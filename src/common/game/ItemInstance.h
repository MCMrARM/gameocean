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
    inline Item* getItem() { return item; };
    void setItem(Item* v) { item = v; nameOverride = ""; };
    inline bool isEmpty() { return (item == null); };
    inline std::string& getNameOverride() { return nameOverride; };
    inline void overrideName(std::string val) { nameOverride = val; };
    std::string getName();

    void setEmpty() {
        item = null;
        nameOverride = "";
        count = 0;
        damage = 0;
    };

    bool isSameType(ItemInstance& i) {
        return (i.item == item && i.damage == i.damage);
    };

    inline bool operator==(const ItemInstance& s) {
        return (item == s.item && count == s.count && damage == s.damage && nameOverride == s.nameOverride);
    };
    inline bool operator!=(const ItemInstance& r) { return !(*this == r); };

};