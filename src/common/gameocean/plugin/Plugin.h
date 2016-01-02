#pragma once

#include <vector>
#include <memory>
#include "PluginInfo.h"

class Server;
class Command;

class Plugin {

    friend class Event;
    friend class PluginManager;
    friend class Command;

private:
    Server* server;
    PluginInfo info;
    std::vector<std::shared_ptr<void>> callbacks;
    std::vector<Command*> commands;

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
    virtual void disable();

    void unregisterEvents() {
        callbacks.clear();
    }

#ifdef SERVER
    void unregisterCommands();
#endif

};