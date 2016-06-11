#pragma once

#include <sstream>
#include "../Command.h"
#include <gameocean/plugin/event/entity/EntityDamageEvent.h>

class KillCommand : public IngameCommand {

public:
    virtual std::string getName() { return "kill"; };
    virtual std::string getDescription() { return "Kills you"; };
    virtual std::string getUsage() { return "/kill"; };
    virtual Permission *getRequiredPermission() { return Permission::suicideCommand; };

    KillCommand(Server &server) : IngameCommand(server) { };

    virtual void process(Player &sender, std::vector<std::string> args) {
        EntityDamageEvent ev (sender, 10000.f, EntityDamageEvent::DamageSource::SUICIDE);
        sender.damage(ev);
    };

};


