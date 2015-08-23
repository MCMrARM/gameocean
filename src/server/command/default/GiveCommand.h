#pragma once

#include <sstream>
#include "common.h"
#include "../Command.h"
#include "game/Item.h"

class GiveCommand : public Command {

public:
    virtual std::string getName() { return "give"; };
    virtual std::string getDescription() { return "Gives an item"; };
    virtual std::string getUsage() { return "/give <player> <item[:damage]> [count]"; };

    GiveCommand(Server& server) : Command(server) { };

    virtual void process(CommandSender& sender, std::vector<std::string> args) {
        if (args.size() < 3 || args[1].length() <= 0 || args[2].length() <= 0) {
            sendUsage(sender);
            return;
        }
        Player* player = server.findPlayer(args[1]);
        if (player == null) {
            sender.sendMessage("Specified player could't be found!");
            return;
        }

        byte damage = 0;
        std::string itemName = args[2];
        {
            size_t i = itemName.find(":");
            if (i != std::string::npos) {
                std::string dmgStr = itemName.substr(i + 1);
                itemName = itemName.substr(0, i);
                try {
                    int dmgI = std::stoi(dmgStr);
                    if (dmgI >= 0 && dmgI <= UINT8_MAX)
                        damage = dmgI;
                } catch (std::exception e) {
                }
            }
        }

        Item* item = Item::getItem(itemName);
        if (item == null) {
            sender.sendMessage("Specified item doesn't exist!");
            return;
        }

        int count = item->getMaxStackSize();
        if (args.size() > 3) {
            try {
                count = std::stoi(args[3]);
            } catch (std::exception e) {
            }
        }

        if (count <= 0) {
            sender.sendMessage("Please specify a valid item amount!");
            return;
        }

        player->inventory.addItem(ItemInstance (item, count, damage));
    };

};
