#pragma once

#include <sstream>
#include "../Command.h"

class PingCommand : public IngameCommand {

public:
    virtual std::string getName() { return "ping"; };
    virtual std::string getDescription() { return "Tells you your current ping to this server"; };
    virtual std::string getUsage() { return "/ping"; };
    virtual Permission *getRequiredPermission() { return Permission::pingCommand; };

    PingCommand(Server &server) : IngameCommand(server) { };

    virtual void process(Player &sender, std::vector<std::string> args) {
        std::stringstream msg;
        msg << "Your current ping is: " << sender.getPing();
        sender.sendMessage(msg.str());
    };

};


