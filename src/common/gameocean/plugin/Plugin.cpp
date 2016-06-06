#include "Plugin.h"


void Plugin::disable() {
#ifdef SERVER
    unregisterPermissions();
    unregisterCommands();
#endif
    unregisterEvents();
}

#ifdef SERVER
#include <gameocean/command/Command.h>
void Plugin::unregisterPermissions() {
    for (std::shared_ptr<Player> p : server->getPlayers()) {
        p->removePermissions(permissions, false);
    }
    for (Permission *perm : permissions) {
        delete perm;
    }
    permissions.clear();
}
void Plugin::unregisterCommands() {
    for (Command *c : commands) {
        Command::unregisterCommand(c);
        delete c;
    }
    commands.clear();
}
#endif