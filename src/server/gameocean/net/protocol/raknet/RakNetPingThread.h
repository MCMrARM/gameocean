#pragma once

#include <gameocean/utils/Thread.h>
class RakNetProtocolServer;

class RakNetPingThread : public Thread {

protected:
    bool shouldStop = false;

public:
    RakNetProtocolServer &server;
    int pingInterval = 5000;

    RakNetPingThread(RakNetProtocolServer &server) : server(server) { };

    virtual void run();
    virtual void stop() { shouldStop = true; };

};

