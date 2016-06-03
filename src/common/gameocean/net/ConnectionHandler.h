#pragma once

#include "Connection.h"

class ConnectionHandler {

public:
    struct ClientAcceptanceStatus {
        bool accepted = true;
        std::string reason;
    };

    /**
     * Only needed for server handler.
     * This function is called when the server gets a connection request.
     */
    virtual ClientAcceptanceStatus acceptClient(Connection &connection) {
        return ClientAcceptanceStatus ();
    }

    /**
     * This function is called when the Connection instance is accepted. It should be called when the protocol version
     * and details are negotiated.
     */
    virtual void connected(Connection &connection) { }

    /**
     * This function is called when the Connection instance is closed.
     */
    virtual void disconnected(Connection &connection, Connection::DisconnectReason reason, std::string textReason) { }

};