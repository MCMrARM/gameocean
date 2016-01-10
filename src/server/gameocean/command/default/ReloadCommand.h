#pragma once

#include <sstream>
#include "../Command.h"

class ReloadCommand : public Command {

public:
    virtual std::string getName() { return "reload"; };
    virtual std::string getDescription() { return "Reloads the server configuration and plugins"; };
    virtual std::string getUsage() { return "/reload"; };
    virtual Permission* getRequiredPermission() { return Permission::reloadServerCommand; };

    ReloadCommand(Server& server) : Command(server) { };

    virtual void process(CommandSender& sender, std::vector<std::string> args) {
        server.broadcastMessage(std::string("[") + sender.getName() + "] Reloading the server");
        server.reload();
        server.broadcastMessage(std::string("[") + sender.getName() + "] Reloaded the server!");
    };

};
