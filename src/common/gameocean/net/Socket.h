#pragma once

#include "../utils/BinaryStream.h"

class Socket {

public:
    enum class Protocol {
        TCP, UDP
    };
    enum class AddressVersion {
        IPv4, IPv6
    };
    FileBinaryStream stream;

protected:
    int fd = -1;
    Protocol protocol;
    AddressVersion version;

public:
    Socket() {};
    Socket(int fd);
    ~Socket();

    bool connect(std::string ip, unsigned short port, Protocol protocol, AddressVersion version);
    bool connect(std::string ip, unsigned short port, Protocol protocol);

    void close();

    std::string getPeerName();
    int getPeerPort();

    FileBinaryStream& getStream();

};


