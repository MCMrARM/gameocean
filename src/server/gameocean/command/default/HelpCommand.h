#pragma once

#include <sstream>
#include <gameocean/common.h>
#include "../Command.h"

class HelpCommand : public Command {

public:
    static const int COMMANDS_PER_PAGE = 5;

    virtual std::string getName() { return "help"; }
    virtual std::string getDescription() { return "Displays this help"; }
    virtual std::string getUsage() { return "/help [command|page]"; }
    virtual Permission *getRequiredPermission() { return Permission::helpCommand; }

    HelpCommand(Server &server) : Command(server) { }

    virtual void process(CommandSender &sender, std::vector<std::string> args) {
        int page = 0;
        if (args.size() > 1 && args[1].length() > 0) {
            Command *command = Command::getCommand(args[1]);
            if (command != nullptr) {
                sender.sendMessage(command->getUsage() + ": " + command->getDescription());
                return;
            }

            try {
                page = std::stoi(args[1]) - 1;
            } catch (std::exception e) {
            }
        }

        if (Command::commands.size() == 0)
            return;

        auto it = Command::commandSet.begin();
        int availableCommandCount = 0;
        for (auto cmd = Command::commandSet.begin(); cmd != Command::commandSet.end(); cmd++) {
            auto perm = (*cmd)->getRequiredPermission();
            if (perm == nullptr || sender.hasPermission(perm)) {
                if (availableCommandCount % COMMANDS_PER_PAGE == 0 && availableCommandCount / COMMANDS_PER_PAGE == page)
                    it = cmd;
                availableCommandCount++;
            }
        }

        int pages = (availableCommandCount - 1) / COMMANDS_PER_PAGE + 1;
        if (page < 0 || page >= pages) {
            page = 0;
        }
        {
            std::stringstream msg;
            msg << "== Help [" << (page + 1) << "/" << pages << "] ==";
            sender.sendMessage(msg.str());
        }

        for (int i = 0; it != Command::commandSet.end() && i < COMMANDS_PER_PAGE; it++) {
            Command *cmd = *it;
            auto perm = cmd->getRequiredPermission();
            if (perm != nullptr && !sender.hasPermission(perm))
                continue;
            sender.sendMessage(cmd->getName() + ": " + cmd->getDescription());
            i++;
        }
    }

};
