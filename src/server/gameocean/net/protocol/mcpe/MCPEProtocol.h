#pragma once

#include <map>
#include "../raknet/RakNetProtocol.h"
#include "MCPEProtocolServer.h"

class MCPEPacket;
class MCPEPlayer;

class MCPEProtocol : public RakNetProtocol {

protected:
    MCPEProtocolServer server;

public:
    MCPEProtocol();

    virtual std::string getName() { return "MCPE"; }

    static const int CURRENT_VERSION = 60;
    static const char *CURRENT_VERSION_STRING;

    virtual ProtocolServer &getServer() {
        return server;
    }

};