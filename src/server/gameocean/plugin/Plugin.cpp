#include "Plugin.h"

#include "../command/Command.h"

Plugin::~Plugin() {
    for (Command* c : commands) {
        Command::registerCommand(c);
        delete c;
    }
}