#pragma once

#include "../utils/Thread.h"

class Connection;

class ConnectionReceiveThread : public Thread {

private:
    Connection &connection;

    virtual void run();

public:
    ConnectionReceiveThread(Connection &connection) : connection(connection) {
        //
    }

    virtual void stop();

};


