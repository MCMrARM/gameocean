#pragma once

#include "../../Connection.h"

namespace DefaultProtocol {

    class SimpleTCPConnection : public Connection {

    public:
        Socket* socket;

        SimpleTCPConnection(Protocol& protocol, Socket &socket, bool client);
        SimpleTCPConnection(Protocol& protocol, std::string ip, unsigned short port);
        virtual ~SimpleTCPConnection() {
            doClose();
        }

        virtual void doClose();
        virtual void close(DisconnectReason reason, std::string msg = "");

        virtual void send(Packet &packet);
        virtual bool readAndHandlePacket();

    };

}