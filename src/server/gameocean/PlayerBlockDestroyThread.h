#pragma once

#include <gameocean/utils/Thread.h>
#include <condition_variable>
class Server;

class PlayerBlockDestroyThread : public Thread {

protected:
    bool shouldStop = false;
    std::mutex notifyMutex;
    std::condition_variable notify;

public:
    Server& server;

    PlayerBlockDestroyThread(Server& server) : server(server) { };

    void notifyChange() {
        notify.notify_one();
    };

    virtual void run();
    virtual void stop() { shouldStop = true; };

};


