#pragma once

#include <memory>
#include "../utils/Thread.h"

class Connection;

class ConnectionReceiveThread : public Thread {

private:
    std::shared_ptr<Connection> connection;
    bool selfDestroy;

    virtual void run();

public:
    ConnectionReceiveThread(std::shared_ptr<Connection> connection, bool selfDestroy) : connection(connection), selfDestroy(selfDestroy) {
        //
    }
    ~ConnectionReceiveThread() {
        if (selfDestroy)
            delete this;
    }

};


