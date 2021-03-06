#pragma once

#include "Socket.h"
#include "Datagram.h"

class ServerSocket {

protected:
    int fd;

public:
    ServerSocket() { }

    bool listen(std::string ip, unsigned short port, Socket::Protocol protocol, Socket::AddressVersion version = Socket::AddressVersion::IPv4);

    Socket *accept();

    Datagram receiveDatagram();
    void sendDatagram(Datagram const &dg);

    void close();

};


