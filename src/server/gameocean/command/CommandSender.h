#pragma once

#include <string>
#include <iostream>
class Permission;

class CommandSender {

public:
    virtual std::string getName() = 0;

    virtual void sendMessage(std::string text) = 0;

    virtual bool hasPermission(Permission *perm) = 0;

};

class ServerCommandSender : public CommandSender {

public:
    virtual std::string getName() {
        return "Server";
    }

    virtual bool hasPermission(Permission *perm) {
        return true;
    }

    virtual void sendMessage(std::string text) {
        std::cout << text << std::endl;
    }

};