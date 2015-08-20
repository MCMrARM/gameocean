#pragma once

#include <deque>
#include <memory>
#include <functional>
#include "../../Player.h"
#include "MCPEProtocol.h"
#include <RakNet/RakNetTypes.h>
#include "../../world/ChunkPos.h"

class MCPEPacket;
class MCPEPacketBatchThread;
class Chunk;

class MCPEPlayer : public Player {

protected:
    friend class MCPEPacketBatchThread;

    MCPEProtocol& protocol;
    RakNet::RakNetGUID guid;
    RakNet::SystemAddress address;

    std::map<int, std::vector<ChunkPos>> raknetChunkQueue;

    typedef std::function<void(MCPEPlayer*, MCPEPacket*, int)> QueuedPacketCallback;

    struct QueuedPacket {
        MCPEPacket* pk;
        QueuedPacketCallback callback;
    };

    std::deque<QueuedPacket> packetQueue;
    std::mutex packetQueueMutex;

    virtual bool sendChunk(int x, int z);
    virtual void receivedChunk(int x, int z);

    virtual void setSpawned();

    virtual void sendPosition(float x, float y, float z);

    virtual void spawnEntity(Entity* entity);
    virtual void despawnEntity(Entity* entity);
    virtual void updateEntityPos(Entity* entity);

public:
    MCPEPlayer(Server& server, MCPEProtocol& protocol, RakNet::RakNetGUID guid, RakNet::SystemAddress address) : Player(server), protocol(protocol), guid(guid), address(address) {};

    inline RakNet::SystemAddress& getAddress() { return address; };

    int directPacket(MCPEPacket* packet);
    int writePacket(std::unique_ptr<MCPEPacket> packet, bool batch);
    inline int writePacket(std::unique_ptr<MCPEPacket> packet) { return writePacket(std::move(packet), packet->priority); };
    void batchPacketCallback(std::unique_ptr<MCPEPacket> packet, QueuedPacketCallback&& sentCallback);
    void receivedACK(int packetId);

    virtual void sendMessage(std::string text);

};


