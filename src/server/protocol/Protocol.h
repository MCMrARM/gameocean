#pragma once

#include "../Server.h"
#include "utils/Thread.h"
#include "utils/Logger.h"

class Protocol : public Thread {

protected:
    Server& server;
    bool shouldStop = false;

    virtual void run() {
        loop();
    };

public:
    Protocol(Server& server) : server(server) {};

    virtual std::string getName() = 0;

    virtual void start(int port) {
        Thread::start();
        Logger::main->debug("Protocol", "Starting %s protocol", getName().c_str());
    }
    virtual void stop() {
        Logger::main->debug("Protocol", "Stopping %s protocol", getName().c_str());
        shouldStop = true;
    };

    virtual void loop() = 0;

};


