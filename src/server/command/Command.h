#pragma once

#include <unordered_map>
#include <vector>
#include "CommandSender.h"
class Player;

class Command {

private:
    static std::unordered_map<std::string, Command*> commands;

public:
    static void registerCommand(Command* command);
    static void registerDefaultCommands();
    static Command* getCommand(std::string name);

    virtual std::string getName() = 0;
    virtual std::vector<std::string> getAlternativeNames() { return {}; };

    virtual void process(CommandSender& sender, std::vector<std::string> args) = 0;
    virtual void process(Player& sender, std::vector<std::string> args) { process(sender, args); };

};

class IngameCommand : public Command {

public:
    virtual void process(CommandSender& sender, std::vector<std::string> args) {
        sender.sendMessage("You must run this command in-game");
    };
    virtual void process(Player& sender, std::vector<std::string> args) = 0;

};
