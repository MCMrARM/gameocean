#pragma once

#include "Connection.h"

class ConnectionHandler {

public:
    virtual bool isServer() { return false; };
    virtual bool isClient() { return false; };

};

class ServerConnectionHandler : public ConnectionHandler {

public:
    virtual bool isServer() { return true; };

    struct ClientAcceptanceStatus {
        bool accepted = true;
        std::string reason;
    };

    virtual ClientAcceptanceStatus acceptClient(Connection& connection) {
        ClientAcceptanceStatus status;
        return status;
    };

};

class ClientConnectionHandler : public ConnectionHandler {

public:
    virtual bool isClient() { return true; };

    Connection& connection;

    ClientConnectionHandler(Connection& connection) : connection(connection) {};

    virtual void connected() { };
    virtual void disconnected(Connection::DisconnectReason reason, std::string textReason) { };


};