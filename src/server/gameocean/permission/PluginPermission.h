#pragma once

#include "Permission.h"
#include <gameocean/plugin/Plugin.h>

class PluginPermission : public Permission {

public:
    PluginPermission(Plugin* plugin, std::string name, DefaultGrantTo grantTo) : Permission(name, grantTo) {
        plugin->permissions.insert(this);
    }

};