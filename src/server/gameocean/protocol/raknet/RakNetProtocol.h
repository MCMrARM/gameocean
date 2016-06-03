#pragma once

#include <gameocean/net/protocol/Protocol.h>
#include <gameocean/net/protocol/SimpleTCPProtocolServer.h>
#include "RakNetConnection.h"
#include "RakNetProtocolServer.h"

class RakNetProtocol : public Protocol {

public:
    RakNetProtocol();

    virtual Packet *readPacket(BinaryStream &stream, bool client);
    virtual void writePacket(BinaryStream &stream, Packet &packet);

    static bool checkRakNetMagicBytes(char magic[16]);
    static bool checkRakNetMagicBytes(BinaryStream &stream);

    static sockaddr readRakNetAddress(BinaryStream &stream);
    static void writeRakNetAddress(BinaryStream &stream, sockaddr const& addr);
    static inline unsigned int getRakNetAddressSize(sockaddr const& addr) {
        if (addr.sa_family == AF_INET)
            return 1+4+2;
        return 0;
    }

    static long long getTimeForPing();
};