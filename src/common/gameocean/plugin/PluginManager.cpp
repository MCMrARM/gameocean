#include <gameocean/utils/Config.h>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include "PluginManager.h"
#include <dlfcn.h>

Plugin* PluginManager::loadPlugin(std::string name) {
    if (plugins.count(name) > 0)
        return plugins[name];

    PluginInfo i;
    i.dirName = name;

    std::ifstream inStream ("plugins/" + name + "/plugin.yml");
    if (!inStream) {
        Logger::main->error("PluginManager", "Failed to open plugin info: %s", name.c_str());
        return null;
    }
    Config config (inStream);

    i.name = config.getString("name", "");
    i.libFile = config.getString("binary", "");
    std::shared_ptr<ContainerConfigNode> node = config.getContainer("dependencies");
    if (node) {
        for (std::pair<std::string, std::shared_ptr<ConfigNode>> const& dependency : node->val) {
            loadPlugin(dependency.first);
        }
    }

    if (i.libFile.size() <= 0) {
        Logger::main->error("PluginManager", "Plugin (%s) doesn't have binary path set!", name.c_str());
        return null;
    }

    void* handle = dlopen(i.libFile[0] == '/' ? i.libFile.c_str() : ("plugins/" + name + "/" + i.libFile).c_str(), RTLD_LAZY);
    if (handle == null) {
        Logger::main->error("PluginManager", "Failed to open plugin binary: %s (%s): %s", i.libFile.c_str(), name.c_str(), dlerror());
        return null;
    }
    i.handle = handle;
    Plugin* (*initFunc)(PluginManager*) = (Plugin* (*)(PluginManager*)) dlsym(handle, "_Z4initP13PluginManager");
    Plugin* plugin = initFunc(this);
    plugin->setInfo(server, i);
    plugins[name] = plugin;
}

void PluginManager::unloadPlugin(Plugin* plugin) {
    void* handle = plugin->getPluginInfo().handle;
    std::string name = plugin->getPluginInfo().dirName;
    delete plugin;
    if (dlclose(handle) != 0)
        Logger::main->error("PluginManager", "Failed to close handle of %s", name);
    plugins.erase(name);
}

void PluginManager::enablePlugins() {
    for (auto const& e : plugins) {
        e.second->enable();
    }
}

void PluginManager::disablePlugins() {
    for (auto const& e : plugins) {
        e.second->disable();
    }
}

void PluginManager::loadPlugins() {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("plugins/")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            struct stat sb;
            if (ent->d_name[0] == '.' || stat((std::string("plugins/") + ent->d_name).c_str(), &sb) != 0 || !S_ISDIR(sb.st_mode))
                continue;
            Logger::main->debug("PluginManager", "Loading plugin %s", ent->d_name);
            loadPlugin(std::string(ent->d_name));
        }
        closedir(dir);
    }
}

void PluginManager::unloadPlugins() {
    for (auto const& e : plugins) {
        void* handle = e.second->getPluginInfo().handle;
        std::string name = e.second->getPluginInfo().dirName;
        delete e.second;
        if (dlclose(handle) != 0)
            Logger::main->error("PluginManager", "Failed to close handle of %s", name);
    }
    plugins.clear();
}

void PluginManager::doReload() {
    disablePlugins();
    unloadPlugins();
    loadPlugins();
    enablePlugins();
}

Plugin* PluginManager::getPluginByName(std::string name) {
    if (plugins.count(name) > 0)
        return plugins.at(name);
    return null;
}