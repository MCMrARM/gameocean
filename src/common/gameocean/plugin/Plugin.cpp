#include "Plugin.h"


void Plugin::disable() {
#ifdef SERVER
    unregisterCommands();
#endif
    unregisterEvents();
}

#ifdef SERVER
#include <gameocean/command/Command.h>
void Plugin::unregisterCommands() {
    for (Command* c : commands) {
        Command::unregisterCommand(c);
        delete c;
    }
    commands.clear();
}
#endif