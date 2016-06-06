#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include "CommandSender.h"
#include "../Player.h"
#include "../permission/Permission.h"

class Server;
class Plugin;

class Command {

public:
    /**
     * This map links all command names to their respective Command instance. This map includes all command aliases.
     */
    static std::unordered_map<std::string, Command *> commands;

    static std::set<Command *> commandSet;

    /**
     * This function registers a command.
     *
     * Please don't use it to register plugin commands as this function doesn't allow automatical unregister of the
     * command after unloading the plugin and therefore can break reloading.
     */
    static void registerCommand(Command *command);
    static void registerPluginCommand(Plugin *plugin, Command *command);
    static void registerDefaultCommands(Server &server);
    static void unregisterCommand(Command *command);
    static Command *getCommand(std::string name);

    Server &server;
    Command(Server &server) : server(server) { };
    virtual ~Command() { };

    virtual std::string getName() = 0;
    virtual std::string getDescription() { return ""; };
    virtual std::string getUsage() = 0;
    virtual std::vector<std::string> getAlternativeNames() { return {}; };
    virtual Permission *getRequiredPermission() { return nullptr; };

    virtual void process(CommandSender &sender, std::vector<std::string> args) = 0;
    virtual void process(Player &sender, std::vector<std::string> args) { process((CommandSender &) sender, args); };

    void sendUsage(CommandSender &to) {
        to.sendMessage(getUsage());
    };

};

class IngameCommand : public Command {

public:
    IngameCommand(Server &server) : Command(server) { };

    virtual void process(CommandSender &sender, std::vector<std::string> args) {
        sender.sendMessage("You must run this command in-game");
    };
    virtual void process(Player &sender, std::vector<std::string> args) = 0;

};
