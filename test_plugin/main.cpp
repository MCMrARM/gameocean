#include <iostream>

#include <gameocean/Player.h>
#include <gameocean/plugin/Plugin.h>
#include <gameocean/plugin/PluginManager.h>
#include <gameocean/plugin/event/player/ChatEvent.h>
#include <gameocean/utils/Logger.h>

class TestPlugin : public Plugin {

public:
    virtual void enable() {
        Logger::main->debug("TestPlugin", "enable");
        Event::registerCallback<ChatEvent>(this, std::bind(&TestPlugin::onChat, this, std::placeholders::_1));
    }

    void onChat(ChatEvent& event) {
        event.setFormat("%s: %s");
    }

};

Plugin* init(PluginManager* mgr) {
    Logger::main->debug("TestPlugin", "init!");
    return new TestPlugin();
}