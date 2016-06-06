#pragma once

#include <string>
#include <memory>
class ItemVariant;
class ActionHandlerData;

namespace Json { class Value; };

class ItemAction {

protected:
    ItemVariant *item;

public:
    typedef std::unique_ptr<ActionHandlerData> (*ProcessDataHandler)(Json::Value const&);

    template <typename T>
    static void registerAction(std::string name, std::unique_ptr<ActionHandlerData> (*process)(Json::Value const &), bool (*handler)(T &, ActionHandlerData *)) {
        T::handlers[name] = handler;
        T::processHandlers[name] = process;
    }

    template <typename T>
    static void registerAction(std::string name, bool (*handler)(T &, ActionHandlerData *)) {
        T::handlers[name] = handler;
    }

    ItemAction(ItemVariant *item) : item(item) {
        //
    }

    inline ItemVariant *getItemVariant() {
        return item;
    }

};


