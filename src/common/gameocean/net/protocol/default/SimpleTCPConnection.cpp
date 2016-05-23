#include "SimpleTCPConnection.h"
#include "../../Socket.h"
#include "../Protocol.h"
#include "server/ConnectPacket.h"

namespace DefaultProtocol {

    SimpleTCPConnection::SimpleTCPConnection(Protocol& protocol, Socket &socket, bool client) : Connection(protocol, client) {
        this->socket = &socket;
    }

    SimpleTCPConnection::SimpleTCPConnection(Protocol& protocol, std::string ip, unsigned short port) : Connection(protocol) {
        this->socket = new Socket();
        if (!this->socket->connect(ip, port, Socket::Protocol::TCP)) {
            throw new ConnectionFailedException();
        }

        ConnectPacket pk;
        send(pk);
    }

    void SimpleTCPConnection::close() {
        this->socket->close();
    }

    void SimpleTCPConnection::kick(std::string reason) {
        if (client) {
            this->getClientHandler().disconnected(DisconnectReason::KICKED, reason);
        } else {
            DisconnectedPacket pk;
            pk.reason = DisconnectReason::KICKED;
            pk.textReason = reason;
            send(pk);
        }
    }

    void SimpleTCPConnection::send(Packet &packet) {
        protocol.writePacket(socket->getStream(), packet);
    }

    bool SimpleTCPConnection::readAndHandlePacket() {
        Packet* pk = protocol.readPacket(this->socket->getStream(), client);
        if (pk == nullptr)
            return false;
        handlePacket(pk);
        delete pk;
        return true;
    }


}