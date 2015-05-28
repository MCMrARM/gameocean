#include "Connection.h"
#include "Socket.h"
#include "ConnectionHandler.h"
#include "server/ConnectPacket.h"
#include "client/DisconnectedPacket.h"
#include "../utils/Logger.h"

Connection::Connection(Socket& socket, bool client) {
    this->socket = &socket;
    this->client = client;
}

Connection::Connection(std::string ip, unsigned short port) {
    this->socket = new Socket();
    if(!this->socket->connect(ip, port, Socket::Protocol::TCP)) {
        throw new ConnectionFailedException();
    }

    ConnectPacket pk;
    Packet::sendPacket(*this, pk);
}

void Connection::close() {
    this->socket->close();
}

void Connection::kick(std::string reason) {
    if (client) {
        this->getClientHandler().disconnected(DisconnectReason::KICKED, reason);
    } else {
        DisconnectedPacket pk;
        pk.reason = DisconnectReason::KICKED;
        pk.textReason = reason;
        Packet::sendPacket(this->socket->getStream(), pk);
    }
}

void Connection::setHandler(ConnectionHandler &handler) {
    this->handler = &handler;
}

ConnectionHandler& Connection::getHandler() {
    if (handler == null) {
        if (client) {
            handler = new ClientConnectionHandler(*this);
        } else {
            handler = new ServerConnectionHandler();
        }
    }
    return *handler;
}

ClientConnectionHandler& Connection::getClientHandler() {
    if (!this->handler->isClient()) throw new ConnectionTypeException();
    return (ClientConnectionHandler&) *this->handler;
}

ServerConnectionHandler& Connection::getServerHandler() {
    if (!this->handler->isServer()) throw new ConnectionTypeException();
    return (ServerConnectionHandler&) *this->handler;
}

void Connection::handlePacket(Packet *packet) {
    if (client) {
        packet->handleClient(*this);
    } else {
        packet->handleServer(*this);
    }
}

void Connection::handlePacket() {
    Packet* pk = Packet::getPacket(this->socket->getStream(), client);
    handlePacket(pk);
    delete pk;
}

void Connection::loop() {
    while (true) {
        Connection::handlePacket();
    }
}