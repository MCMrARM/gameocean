#pragma once

#include <gameocean/utils/Thread.h>
class RakNetProtocolServer;

class RakNetResendThread : public Thread {

protected:
    bool shouldStop = false;

public:
    RakNetProtocolServer &server;
    int resendDelay = 10; // in ms

    RakNetResendThread(RakNetProtocolServer &server) : server(server) { };

    virtual void run();
    virtual void stop() { shouldStop = true; };

};

