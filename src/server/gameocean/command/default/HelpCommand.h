#pragma once

#include <sstream>
#include <gameocean/common.h>
#include "../Command.h"

class HelpCommand : public Command {

public:
    static const int COMMANDS_PER_PAGE = 5;

    virtual std::string getName() { return "help"; };
    virtual std::string getDescription() { return "Displays this help"; };
    virtual std::string getUsage() { return "/help [command|page]"; };
    virtual Permission* getRequiredPermission() { return Permission::helpCommand; };

    HelpCommand(Server& server) : Command(server) { };

    virtual void process(CommandSender& sender, std::vector<std::string> args) {
        int page = 0;
        if (args.size() > 1 && args[1].length() > 0) {
            Command* command = Command::getCommand(args[1]);
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

        int pages = (int) (Command::commandSet.size() - 1) / COMMANDS_PER_PAGE + 1;
        if (page < 0 || page >= pages) {
            page = 0;
        }
        {
            std::stringstream msg;
            msg << "== Help [" << (page + 1) << "/" << pages << "] ==";
            sender.sendMessage(msg.str());
        }
        int start = page * COMMANDS_PER_PAGE;
        auto it = Command::commandSet.begin();
        std::advance(it, start);

        for (int i = 0; it != Command::commandSet.end() && i < COMMANDS_PER_PAGE; it++, i++) {
            Command* cmd = *it;
            sender.sendMessage(cmd->getName() + ": " + cmd->getDescription());
        }
    };

};
