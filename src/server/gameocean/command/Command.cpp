#include "Command.h"

#include "default/TeleportCommand.h"
#include "default/HelpCommand.h"
#include "default/GiveCommand.h"
#include "default/TimeCommand.h"
#include "default/VersionCommand.h"
#include "default/StopCommand.h"
#include "default/ReloadCommand.h"
#include "default/PingCommand.h"
#include "default/KillCommand.h"
#include <gameocean/utils/StringUtils.h>

std::unordered_map<std::string, Command *> Command::commands;
std::set<Command *> Command::commandSet;

void Command::registerCommand(Command *command) {
    commandSet.insert(command);
    commands[command->getName()] = command;
    for (std::string str : command->getAlternativeNames()) {
        if (commands.count(str) <= 0) {
            commands[str] = command;
        }
    }
}

void Command::unregisterCommand(Command *command) {
    commands.erase(command->getName());
    commandSet.erase(command);
}

void Command::registerDefaultCommands(Server &server) {
    Command::registerCommand(new TeleportCommand(server));
    Command::registerCommand(new HelpCommand(server));
    Command::registerCommand(new GiveCommand(server));
    Command::registerCommand(new TimeCommand(server));
    Command::registerCommand(new VersionCommand(server));
    Command::registerCommand(new StopCommand(server));
    Command::registerCommand(new ReloadCommand(server));
    Command::registerCommand(new PingCommand(server));
    Command::registerCommand(new KillCommand(server));
}

void Command::registerPluginCommand(Plugin *plugin, Command *command) {
    registerCommand(command);
    plugin->commands.push_back(command);
}

Command *Command::getCommand(std::string name) {
    if (commands.count(name) > 0) {
        return commands.at(name);
    }
    return nullptr;
}