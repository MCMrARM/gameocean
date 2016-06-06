#pragma once

#include <sstream>
#include "../Command.h"

class StopCommand : public Command {

public:
    virtual std::string getName() { return "stop"; };
    virtual std::string getDescription() { return "Stops the server"; };
    virtual std::string getUsage() { return "/stop"; };
    virtual Permission *getRequiredPermission() { return Permission::stopServerCommand; };

    StopCommand(Server &server) : Command(server) { };

    virtual void process(CommandSender &sender, std::vector<std::string> args) {
        server.broadcastMessage(std::string("[") + sender.getName() + "] Stopping the server...");
        server.stop();
    };

};


