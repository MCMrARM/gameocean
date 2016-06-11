#pragma once

#include <gameocean/utils/Thread.h>

class Server;

class PlayerChunkQueueThread : public Thread {

protected:
    bool shouldStop = false;

public:
    Server &server;

    PlayerChunkQueueThread(Server &server) : server(server) { }

    virtual void run();
    virtual void stop() { shouldStop = true; }

};


