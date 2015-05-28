#pragma once

#include <string>
#include <exception>
class Socket;
class Packet;
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

public:
    Socket* socket;
    ConnectionHandler* handler;

    bool accepted = false;
    bool client = true;

    Connection(Socket& socket, bool client);
    Connection(std::string ip, unsigned short port);

    enum class DisconnectReason {
        // login
        WRONG_GAME, OUTDATED_CLIENT, OUTDATED_SERVER, OUTDATED_GAME, OUTDATED_GAME_SERVER,
        // kick
        KICKED,
        // socket
        TIMEOUT, CLOSED, UNKNOWN
    };

    void close();
    void kick(std::string reason);

    void setHandler(ConnectionHandler& handler);

    ConnectionHandler& getHandler();
    ClientConnectionHandler& getClientHandler();
    ServerConnectionHandler& getServerHandler();

    void handlePacket(Packet* packet);
    void handlePacket();
    void loop();

};


