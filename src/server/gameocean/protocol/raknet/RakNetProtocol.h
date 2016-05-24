#pragma once

#include <gameocean/net/protocol/Protocol.h>
#include <gameocean/net/protocol/SimpleTCPProtocolServer.h>
#include "RakNetConnection.h"
#include "RakNetProtocolServer.h"

class RakNetProtocol : public Protocol {

public:
    RakNetProtocol() {}

    virtual Packet *readPacket(BinaryStream &stream, bool client);
    virtual void writePacket(BinaryStream &stream, const Packet &packet);

    static bool checkRakNetMagicBytes(char magic[16]);
    static bool checkRakNetMagicBytes(BinaryStream &stream);
};