#pragma once

#include <sstream>
#include "../Command.h"
#include <gameocean/world/World.h>
#include <gameocean/utils/StringUtils.h>

class TimeCommand : public IngameCommand {

public:
    virtual std::string getName() { return "time"; }
    virtual std::string getDescription() { return "Sets the time"; }
    virtual std::string getUsage() { return "/time <start|stop|set <time>>"; }
    virtual Permission *getRequiredPermission() { return Permission::worldTimeCommand; }

    TimeCommand(Server &server) : IngameCommand(server) { }

    virtual void process(Player &sender, std::vector<std::string> args) {
        if (args.size() < 2) {
            sendUsage(sender);
            return;
        }
        args[1] = StringUtils::toLowercase(args[1]);
        if (args[1] == "start") {
            server.broadcastMessage(std::string("[") + sender.getName() + "] Started time");
            sender.getWorld().setTimeStopped(false);
        } else if (args[1] == "stop") {
            server.broadcastMessage(std::string("[") + sender.getName() + "] Stopped time");
            sender.getWorld().setTimeStopped(true);
        } else if (args[1] == "set") {
            if (args.size() < 3) {
                sendUsage(sender);
                return;
            }
            int time = StringUtils::asInt(args[2], 0);
            std::stringstream msg;
            msg << "[" << sender.getName() << "] Set time to " << time;
            server.broadcastMessage(msg.str());
            sender.getWorld().setTime(time, sender.getWorld().isTimeStopped());
        } else {
            sendUsage(sender);
        }
    }

};
