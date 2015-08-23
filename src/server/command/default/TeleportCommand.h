#pragma once

#include <iostream>
#include <sstream>
#include "../Command.h"

class TeleportCommand : public IngameCommand {

public:
    virtual std::string getName() { return "tp"; };
    virtual std::string getDescription() { return "Teleports you to the specified coordinates"; };
    virtual std::string getUsage() { return "/tp <x> <y> <z>"; };

    TeleportCommand(Server& server) : IngameCommand(server) { };

    virtual void process(Player& sender, std::vector<std::string> args) {
        try {
            if (args.size() >= 4) {
                int x = std::stoi(args[1]);
                int y = std::stoi(args[2]);
                int z = std::stoi(args[3]);

                std::stringstream ss;
                ss << "You were teleported to: [" << x << ", " << y << ", " << z << "]";
                sender.sendMessage(ss.str());
                sender.teleport(std::stoi(args[1]), std::stoi(args[2]), std::stoi(args[3]));
            } else {
                sendUsage(sender);
            }
        } catch (std::exception e) {
            sender.sendMessage("Couldn't teleport you to the specified position!");
        }
    };

};


