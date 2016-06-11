#pragma once

#include <gameocean/common.h>
#include <string>

class ItemVariant;
class BlockVariant;

class ItemInstance {
protected:
    ItemVariant *item = nullptr;
    short data = 0;
    std::string nameOverride;

public:
    byte count = 1;

    ItemInstance() : count(0) { }
    ItemInstance(ItemVariant *item);
    ItemInstance(ItemVariant *item, byte count) : ItemInstance(item) { this->count = count; }
    ItemInstance(int id, short data) { setItem(id, data); }
    ItemInstance(ItemVariant *item, byte count, short data) : item(item), count(count), data(data) { }
    ItemInstance(int id, byte count, short data) : count(count) { setItem(id, data); }

    inline ItemVariant *getItem() { return item; }
    void setItem(ItemVariant *v, short data) { item = v; this->data = data; nameOverride = ""; }
    void setItem(int id, short damage);
    int getItemId() const;
    inline void setItemId(int id) { setItem(id, data); }
    inline short getItemData() const { return data; }
    void setItemDamage(short damage);
    void damageItem(int points);

    inline bool isEmpty() const { return (item == nullptr); }
    inline std::string &getNameOverride() { return nameOverride; }
    inline void overrideName(std::string val) { nameOverride = val; }
    std::string getName() const;

    void setEmpty() {
        item = nullptr;
        nameOverride = "";
        count = 0;
        data = 0;
    }

    bool isSameType(const ItemInstance &i) const {
        return (i.item == item);
    }

    inline bool operator==(const ItemInstance &s) {
        return (item == s.item && count == s.count && data == s.data && nameOverride == s.nameOverride);
    }
    inline bool operator!=(const ItemInstance &r) { return !(*this == r); }

};