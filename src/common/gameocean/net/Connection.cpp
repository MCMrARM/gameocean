#include "Connection.h"
#include "Socket.h"
#include "Packet.h"
#include "ConnectionHandler.h"
#include "../utils/Logger.h"

void Connection::setHandler(ConnectionHandler &handler) {
    this->handler = &handler;
}

ConnectionHandler& Connection::getHandler() {
    if (handler == nullptr) {
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

void Connection::loop() {
    while (true) {
        if (!readAndHandlePacket())
            break;
    }
}

void Connection::close(DisconnectReason reason, std::string msg) {
    if (client) {
        this->getClientHandler().disconnected(reason, msg);
    } else {
        this->getServerHandler().disconnected(*this, reason, msg);
    }
    close();
}

void Connection::kick(std::string reason) {
    close(DisconnectReason::KICKED, reason);
}