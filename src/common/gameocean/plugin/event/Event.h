#pragma once

#include <functional>
#include "CallbackList.h"
#include "../Plugin.h"

class Event {

public:
    template<typename T>
    static void registerCallback(Plugin *plugin, std::function<void (T &)> event) {
        std::shared_ptr<CallbackAuto<T>> callback (new CallbackAuto<T>(event));
        plugin->callbacks.push_back(callback);
    }

    template<typename T>
    static void broadcast(T &event) {
        T::callbacks.call(event);
    }

};