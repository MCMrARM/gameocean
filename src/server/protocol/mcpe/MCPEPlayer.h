#pragma once

#include "../../Player.h"
#include "MCPEProtocol.h"
#include "../../libs/RakNet/RakNetTypes.h"
#include "../../world/ChunkPos.h"

class MCPEPacket;
class Chunk;

class MCPEPlayer : public Player {

protected:
    MCPEProtocol& protocol;
    RakNet::RakNetGUID guid;
    RakNet::SystemAddress address;

    std::map<int, ChunkPos> raknetChunkQueue;

    virtual bool sendChunk(int x, int z);
    virtual void receivedChunk(int x, int z);

public:
    MCPEPlayer(Server& server, MCPEProtocol& protocol, RakNet::RakNetGUID guid, RakNet::SystemAddress address) : Player(server), protocol(protocol), guid(guid), address(address) {};

    inline RakNet::SystemAddress& getAddress() { return address; };

    int writePacket(MCPEPacket &packet);
    void receivedACK(int packetId);

    virtual void sendMessage(std::string text);

};


