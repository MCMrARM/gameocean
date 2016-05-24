#pragma once

#include <gameocean/net/Connection.h>
/*
class RakNetConnection : public Connection {

public:
    RakNetConnection(Protocol& protocol, std::string ip, unsigned short port);
    virtual ~RakNetConnection() {
        close();
    }

    virtual void close();
    virtual void close(DisconnectReason reason, std::string msg = "");

    virtual void send(Packet &packet);
    virtual bool readAndHandlePacket();

};


*/