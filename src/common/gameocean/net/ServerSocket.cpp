#include "ServerSocket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

bool ServerSocket::listen(std::string ip, unsigned short port, Socket::Protocol protocol,
                          Socket::AddressVersion version) {
    sa_family_t family = (sa_family_t)(version == Socket::AddressVersion::IPv6 ? AF_INET6 : AF_INET);
    fd = socket(family, protocol == Socket::Protocol::TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (fd < 0) {
        Logger::main->error("ServerSocket", "Unable to open socket");
        return false;
    }
    {
        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    }
    sockaddr_in addr;
    memset((void*) &addr, 0, sizeof(addr));
    if (ip.size() > 0) {
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
    } else {
        addr.sin_addr.s_addr = INADDR_ANY;
    }
    addr.sin_family = family;
    addr.sin_port = htons(port);
    if (bind(fd, (sockaddr *) &addr, sizeof(addr)) < 0) {
        Logger::main->error("ServerSocket", "Failed to bind socket");
        return false;
    }
    if (protocol == Socket::Protocol::TCP)
        ::listen(fd, 25);
    return true;
}

Socket *ServerSocket::accept() {
    if (fd == 0)
        return nullptr;
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocketId = ::accept(fd, (sockaddr *) &clientAddr, &clientAddrLen);
    if (clientSocketId < 0) {
        Logger::main->error("ServerSocket", "Failed to accept socket");
        return nullptr;
    }
    Logger::main->trace("ServerSocket", "Accepted socket from: %s:%i", inet_ntoa(clientAddr.sin_addr), htons(clientAddr.sin_port));
    return new Socket(clientSocketId);
}

Datagram ServerSocket::receiveDatagram() {
    Datagram ret;
    ret.valid = false;
    socklen_t clientAddrLen = sizeof(ret.addr);
    ret.dataSize = (ssize_t) recvfrom(fd, ret.data, sizeof(ret.data), 0, (sockaddr*) &ret.addr, &clientAddrLen);
    if (ret.dataSize > 0) {
        ret.valid = true;
    }
    return ret;
}

void ServerSocket::sendDatagram(Datagram const &dg) {
    sendto(fd, dg.data, (size_t) dg.dataSize, 0, (sockaddr*) &dg.addr, sizeof(dg.addr));
}

void ServerSocket::close() {
    shutdown(fd, SHUT_RDWR);
    ::close(fd);
}