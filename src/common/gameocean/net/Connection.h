#pragma once

#include <string>
#include <exception>
class Socket;
class Packet;
class Protocol;
class ConnectionHandler;
class ClientConnectionHandler;
class ServerConnectionHandler;

class ConnectionException : public std::exception {
};

class ConnectionFailedException : public ConnectionException {

public:
    virtual const char *what() const throw() {
        return "Failed to connect to the specified server";
    }

};

class ConnectionTypeException : public ConnectionException {

public:
    virtual const char *what() const throw() {
        return "The connection is of a different type";
    }

};

class Connection {

protected:
    bool accepted;
    Protocol& protocol;

public:
    ConnectionHandler* handler;

    const bool client;

    Connection(Protocol& protocol, bool client) : protocol(protocol), client(client) {
        //
    }
    Connection(Protocol& protocol) : Connection(protocol, true) {
        //
    }

    enum class DisconnectReason {
        // login
        WRONG_GAME, OUTDATED_CLIENT, OUTDATED_SERVER, OUTDATED_GAME, OUTDATED_GAME_SERVER,
        // kick
        KICKED,
        // socket
        TIMEOUT, CLOSED, UNKNOWN
    };

    virtual void close() = 0;
    virtual void kick(std::string reason) = 0;

    void setHandler(ConnectionHandler& handler);
    virtual void setAccepted(bool accepted) {
        this->accepted = accepted;
    }

    virtual void send(Packet& packet) = 0;

    inline bool isAccepted() {
        return accepted;
    }

    ConnectionHandler& getHandler();
    ClientConnectionHandler& getClientHandler();
    ServerConnectionHandler& getServerHandler();

    virtual void handlePacket(Packet* packet);
    virtual bool readAndHandlePacket() = 0;
    virtual void loop();

};


