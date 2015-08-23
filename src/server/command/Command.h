#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include "CommandSender.h"
#include "../Player.h"

class Server;

class Command {

public:
    static std::unordered_map<std::string, Command*> commands;
    static void registerCommand(Command* command);
    static void registerDefaultCommands(Server& server);
    static Command* getCommand(std::string name);

    Server& server;
    Command(Server& server) : server(server) { };

    virtual std::string getName() = 0;
    virtual std::string getDescription() { return ""; };
    virtual std::string getUsage() = 0;
    virtual std::vector<std::string> getAlternativeNames() { return {}; };

    virtual void process(CommandSender& sender, std::vector<std::string> args) = 0;
    virtual void process(Player& sender, std::vector<std::string> args) { process((CommandSender&) sender, args); };

    void sendUsage(CommandSender& to) {
        to.sendMessage(getUsage());
    };

};

class IngameCommand : public Command {

public:
    IngameCommand(Server& server) : Command(server) { };

    virtual void process(CommandSender& sender, std::vector<std::string> args) {
        sender.sendMessage("You must run this command in-game");
    };
    virtual void process(Player& sender, std::vector<std::string> args) = 0;

};
