#pragma once

#include <map>
#include "../Protocol.h"
#include <RakNet/RakNetTypes.h>

namespace RakNet {
    class Packet;
    class RakPeerInterface;
    class BitStream;
};

class MCPEPacket;
class MCPEPlayer;

class MCPEProtocol : public Protocol {

protected:
    RakNet::RakPeerInterface* peer;
    std::map<RakNet::RakNetGUID, MCPEPlayer*> players;

    void handlePacket(RakNet::Packet *raknetPacket, MCPEPacket& packet);

public:
    MCPEProtocol(Server& server) : Protocol(server) {};

    static const int CURRENT_VERSION = 34;

    inline RakNet::RakPeerInterface* getPeer() { return peer; };

    virtual void bind(int port);
    virtual void loop();
    virtual void processPacket(RakNet::Packet* packet);

};