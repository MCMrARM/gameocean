#pragma once

#include "../Server.h"

class Protocol {

protected:
    Server& server;

public:
    Protocol(Server& server) : server(server) {};

    virtual void bind(int port) = 0;
    virtual void loop() = 0;

};


