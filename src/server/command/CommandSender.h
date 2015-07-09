#pragma once

#include <string>

class CommandSender {

public:
    virtual std::string getName() = 0;

    virtual void sendMessage(std::string message) = 0;

};