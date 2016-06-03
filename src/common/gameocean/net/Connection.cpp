#include "Connection.h"
#include "Socket.h"
#include "Packet.h"
#include "ConnectionHandler.h"
#include "../utils/Logger.h"

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
    if (hasHandler())
        getHandler()->disconnected(*this, reason, msg);

    close();
}

void Connection::setAccepted(bool accepted) {
    if (accepted && !this->accepted && hasHandler()) {
        getHandler()->connected(*this);
    }
    this->accepted = accepted;
}

void Connection::kick(std::string reason) {
    close(DisconnectReason::KICKED, reason);
}