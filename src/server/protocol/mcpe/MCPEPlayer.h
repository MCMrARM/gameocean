#pragma once

#include "../../Player.h"
#include "MCPEProtocol.h"
#include "../../libs/RakNet/RakNetTypes.h"

class MCPEPacket;

class MCPEPlayer : public Player {

protected:
    MCPEProtocol& protocol;
    RakNet::RakNetGUID guid;
    RakNet::SystemAddress address;

public:
    MCPEPlayer(Server& server, MCPEProtocol& protocol, RakNet::RakNetGUID guid, RakNet::SystemAddress address) : Player(server), protocol(protocol), guid(guid), address(address) {};

    inline RakNet::SystemAddress& getAddress() { return address; };

    void writePacket(MCPEPacket &packet);

};


