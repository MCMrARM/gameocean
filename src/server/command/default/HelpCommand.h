#pragma once

#include <sstream>
#include "common.h"
#include "../Command.h"

class HelpCommand : public Command {

public:
    static const int COMMANDS_PER_PAGE = 5;

    virtual std::string getName() { return "help"; };
    virtual std::string getDescription() { return "Displays this help"; };
    virtual std::string getUsage() { return "/help [command|page]"; };

    HelpCommand(Server& server) : Command(server) { };

    virtual void process(CommandSender& sender, std::vector<std::string> args) {
        int page = 0;
        if (args.size() > 1 && args[1].length() > 0) {
            Command* command = Command::getCommand(args[1]);
            if (command != null) {
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

        int pages = (Command::commands.size() - 1) / COMMANDS_PER_PAGE + 1;
        if (page < 0 || page >= pages) {
            page = 0;
        }
        {
            std::stringstream msg;
            msg << "== Help [" << (page + 1) << "/" << pages << "] ==";
            sender.sendMessage(msg.str());
        }
        int start = page * COMMANDS_PER_PAGE;
        auto it = Command::commands.begin();
        for (int i = 0; i < start; i++)
            it++;

        for (int i = 0; it != Command::commands.end() && i < COMMANDS_PER_PAGE; it++, i++) {
            Command* cmd = it->second;
            if (cmd == null)
                return;
            sender.sendMessage(cmd->getName() + ": " + cmd->getDescription());
        }
    };

};
