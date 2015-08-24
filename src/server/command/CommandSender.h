#pragma once

#include <string>
#include <iostream>

class CommandSender {

public:
    virtual std::string getName() = 0;

    virtual void sendMessage(std::string text) = 0;

};

class ServerCommandSender : public CommandSender {

public:
    virtual std::string getName() {
        return "Server";
    };

    virtual void sendMessage(std::string text) {
        std::cout << text << std::endl;
    };

};