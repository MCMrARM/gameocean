#pragma once

#include <sstream>
#include "../Command.h"
#include <gameocean/GameInfo.h>

class VersionCommand : public Command {

public:
    virtual std::string getName() { return "version"; }
    virtual std::string getDescription() { return "Shows info about this server software"; }
    virtual std::string getUsage() { return "/version"; }
    virtual Permission *getRequiredPermission() { return Permission::serverVersionCommand; }

    VersionCommand(Server &server) : Command(server) { }

    virtual void process(CommandSender &sender, std::vector<std::string> args) {
        sender.sendMessage("This server is running: " + GameInfo::current->name + " " + GameInfo::current->version.toString());
    }

};


