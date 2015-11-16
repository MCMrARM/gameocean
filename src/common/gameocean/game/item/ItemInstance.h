#pragma once

#include <gameocean/common.h>
#include <string>

class ItemVariant;
class BlockVariant;

class ItemInstance {
protected:
    ItemVariant* item = null;
    short data = 0;
    std::string nameOverride;

public:
    byte count = 1;

    ItemInstance() {};
    ItemInstance(ItemVariant* item, short data) : item(item), data(data) { };
    ItemInstance(int id, short data) { setItem(id, data); };
    ItemInstance(ItemVariant* item, byte count, short data) : item(item), count(count), data(data) {};
    ItemInstance(int id, byte count, short data) : count(count) { setItem(id, data); };

    inline ItemVariant* getItem() { return item; };
    void setItem(ItemVariant* v, short data) { item = v; this->data = data; nameOverride = ""; };
    void setItem(int id, short damage);
    int getItemId();
    inline void setItemId(int id) { setItem(id, data); };
    inline short getItemData() { return data; };
    void setItemDamage(short damage);

    inline bool isEmpty() { return (item == null); };
    inline std::string& getNameOverride() { return nameOverride; };
    inline void overrideName(std::string val) { nameOverride = val; };
    std::string getName();

    void setEmpty() {
        item = null;
        nameOverride = "";
        count = 0;
        data = 0;
    };

    bool isSameType(ItemInstance& i) {
        return (i.item == item && i.data == data);
    };

    inline bool operator==(const ItemInstance& s) {
        return (item == s.item && count == s.count && data == s.data && nameOverride == s.nameOverride);
    };
    inline bool operator!=(const ItemInstance& r) { return !(*this == r); };

};