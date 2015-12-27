#include "Plugin.h"

#include "../command/Command.h"

void Plugin::disable() {
    unregisterCommands();
    unregisterEvents();
}

void Plugin::unregisterCommands() {
    for (Command* c : commands) {
        Command::unregisterCommand(c);
        delete c;
    }
    commands.clear();
}