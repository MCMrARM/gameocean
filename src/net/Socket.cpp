#include "Socket.h"

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../utils/Logger.h"

Socket::~Socket() {
    this->close();
}

bool Socket::connect(std::string ip, unsigned short port, Socket::Protocol protocol, AddressVersion version) {
    int family = (version == AddressVersion::IPv6 ? AF_INET6 : AF_INET);
    fd = socket(family, protocol == Protocol::TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (fd < 0) {
        Logger::main->error("Socket", "Unable to open socket");
        return false;
    }

    sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_family = family;
    addr.sin_port = htons(port);

    if (::connect(fd, (sockaddr*) &addr, sizeof(addr)) < 0) {
        Logger::main->error("Socket", "Unable to connect to: %s", ip.c_str());
        return false;
    }

    this->stream.setFileDescriptor(fd);

    return true;
}

bool Socket::connect(std::string ip, unsigned short port, Socket::Protocol protocol) {
    AddressVersion version = AddressVersion::IPv4;
    if (ip.find("::") != std::string::npos) {
        version = AddressVersion::IPv6;
    }
    return Socket::connect(ip, port, protocol, version);
}

void Socket::close() {
    if (fd == -1) return;
    ::close(fd);
    fd = -1;
}

FileBinaryStream& Socket::getStream() {
    return this->stream;
}