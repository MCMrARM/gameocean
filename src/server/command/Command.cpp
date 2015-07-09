#include "Command.h"

#include "default/TeleportCommand.h"

std::unordered_map<std::string, Command*> Command::commands;

void Command::registerCommand(Command* command) {
    commands[command->getName()] = command;
    for (std::string str : command->getAlternativeNames()) {
        if (commands.count(str) <= 0) {
            commands[str] = command;
        }
    }
}

void Command::registerDefaultCommands() {
    Command::registerCommand(new TeleportCommand());
}

Command* Command::getCommand(std::string name) {
    if (commands.count(name) > 0) {
        return commands.at(name);
    }
    return null;
}