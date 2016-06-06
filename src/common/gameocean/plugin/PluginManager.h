#pragma once

#include <set>
#include <map>
#include <string>
#include <memory>
#include "Plugin.h"
#include "PluginInfo.h"

class PluginManager {

private:
    Server &server;
    std::map<std::string, Plugin *> plugins;

public:
    PluginManager(Server &server) : server(server) {
        //
    }

    void loadPlugins();
    void unloadPlugins();
    void enablePlugins();
    void disablePlugins();

    Plugin *loadPlugin(std::string plugin);
    void unloadPlugin(Plugin *plugin);

    Plugin *getPluginByName(std::string name);

    void doReload();

};


