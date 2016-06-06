#pragma once

#include <sstream>
#include <gameocean/common.h>
#include "../Command.h"
#include <gameocean/item/ItemVariant.h>
#include <gameocean/item/ItemRegister.h>

class GiveCommand : public Command {

public:
    virtual std::string getName() { return "give"; };
    virtual std::string getDescription() { return "Gives an item"; };
    virtual std::string getUsage() { return "/give <player> <item[:damage]> [count]"; };
    virtual Permission *getRequiredPermission() { return Permission::giveItemCommand; };

    GiveCommand(Server &server) : Command(server) { };

    virtual void process(CommandSender &sender, std::vector<std::string> args) {
        if (args.size() < 3 || args[1].length() <= 0 || args[2].length() <= 0) {
            sendUsage(sender);
            return;
        }
        std::shared_ptr<Player> player = server.findPlayer(args[1]);
        if (player == nullptr) {
            sender.sendMessage("Specified player could't be found!");
            return;
        }

        short damage = 0;
        std::string itemName = args[2];
        {
            size_t i = itemName.find(":");
            if (i != std::string::npos) {
                std::string dmgStr = itemName.substr(i + 1);
                try {
                    int dmgI = std::stoi(dmgStr);
                    if (dmgI >= 0 && dmgI <= UINT16_MAX)
                        damage = dmgI;
                } catch (std::exception e) {
                }
            }
        }

        ItemVariant *item = ItemRegister::findItem(itemName);
        if (item == nullptr) {
            sender.sendMessage("Specified item doesn't exist!");
            return;
        }

        if (item->getVariantDataId() >= 0) {
            damage = item->getVariantDataId();
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

        ItemInstance itemInstance (item, count, damage);
        player->inventory.addItem(itemInstance);

        std::stringstream msg;
        msg << "Given " << count << " x " << itemInstance.getName() << " to " << player->getName();
        sender.sendMessage(msg.str());
    };

};
