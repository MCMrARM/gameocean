#pragma once

#include <vector>
#include <set>
#include <memory>
#include "PluginInfo.h"

#ifdef SERVER
class Server;
class Command;
class Permission;
#endif

class Plugin {

    friend class Event;
    friend class PluginManager;
    friend class Command;
    friend class PluginPermission;

private:
#ifdef SERVER
    Server* server;
#endif
    PluginInfo info;
    std::vector<std::shared_ptr<void>> callbacks;
#ifdef SERVER
    std::set<Permission*> permissions;
    std::vector<Command*> commands;

    void setInfo(Server& server, PluginInfo info) {
        this->server = &server;
        this->info = info;
    }

protected:
    inline Server& getServer() {
        return *server;
    }
#endif

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
    void unregisterPermissions();
    void unregisterCommands();
#endif

};