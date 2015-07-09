#pragma once

#include <iostream>
#include <sstream>
#include "../Command.h"
#include "../../Player.h"

class TeleportCommand : public IngameCommand {

public:
    virtual std::string getName() { return "tp"; };

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
                sender.sendMessage("Please specify coordinates.");
            }
        } catch (std::exception e) {
            sender.sendMessage("Couldn't teleport you to the specified position!");
        }
    };

};


