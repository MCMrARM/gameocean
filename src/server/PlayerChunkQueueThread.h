#pragma once

#include "utils/Thread.h"

class Server;

class PlayerChunkQueueThread : public Thread {

public:
    Server& server;

    PlayerChunkQueueThread(Server& server) : server(server) { };

    virtual void run();

};


