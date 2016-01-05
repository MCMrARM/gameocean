#pragma once

#include <gameocean/utils/Thread.h>

class Server;

class EntityPhysicsTickTask : public Thread {

protected:
    bool shouldStop = false;

public:
    Server& server;

    EntityPhysicsTickTask(Server& server) : server(server) { };

    virtual void run();
    virtual void stop() { shouldStop = true; };

};


