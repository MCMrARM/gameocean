#include "RakNetConnection.h"
#include <gameocean/net/Socket.h>

/*RakNetConnection::RakNetConnection(Protocol& protocol, Socket &socket, bool client) : Connection(protocol, client) {
    this->socket = &socket;
}

RakNetConnection::RakNetConnection(Protocol& protocol, std::string ip, unsigned short port) : Connection(protocol) {
    this->socket = new Socket();
    if (!this->socket->connect(ip, port, Socket::Protocol::TCP)) {
        throw new ConnectionFailedException();
    }
}*/