#pragma once

#include <vector>
#include <memory>
#include "PluginInfo.h"

class Server;

class Plugin {

    friend class Event;
    friend class PluginManager;

private:
    Server* server;
    PluginInfo info;
    std::vector<std::shared_ptr<void>> callbacks;

    void setInfo(Server& server, PluginInfo info) {
        this->server = &server;
        this->info = info;
    }

protected:
    inline Server& getServer() {
        return *server;
    }

public:
    virtual ~Plugin() { };

    inline PluginInfo& getPluginInfo() {
        return info;
    }

    virtual void enable() { };
    virtual void disable() { };

    void unregisterEvents() {
        callbacks.clear();
    }

};