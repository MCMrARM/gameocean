#pragma once

#include <string>
class ItemVariant;

class ItemAction {

protected:
    ItemVariant* item;

public:
    template <typename T>
    static void registerAction(std::string name, bool (*handler)(T&)) {
        T::handlers[name] = handler;
    }

    ItemAction(ItemVariant* item) : item(item) {
        //
    }

    inline ItemVariant* getItemVariant() {
        return item;
    }

};


