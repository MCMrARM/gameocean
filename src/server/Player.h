#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include "common.h"
#include "Entity.h"
#include "Server.h"
#include "world/ChunkPos.h"
#include "command/CommandSender.h"
#include "protocol/mcpe/MCPEPacket.h"

class Server;
class Protocol;
class Chunk;

class Player : public Entity, public CommandSender {

protected:
    Server& server;

    bool spawned = false;
    bool teleporting = false;
    int viewChunks = 94;

    std::atomic<bool> shouldUpdateChunkQueue;
    std::recursive_mutex chunkArrayMutex;
    std::unordered_map<ChunkPos, Chunk*> sentChunks;
    std::unordered_map<ChunkPos, Chunk*> receivedChunks;
    std::vector<ChunkPos> sendChunksQueue;

    virtual bool sendChunk(int x, int z);
    virtual void receivedChunk(int x, int z);

    virtual void sendPosition(float x, float y, float z) = 0;

public:
    Player(Server& server) : Entity(*server.mainWorld), server(server), shouldUpdateChunkQueue(false) {};

    virtual std::string getName() { return "Player"; };

    virtual void setPos(float x, float y, float z);
    void teleport(float x, float y, float z);

    bool tryMove(float x, float y, float z);

    void updateChunkQueue();
    void sendQueuedChunks();
    void updateTeleportState();

    virtual void sendMessage(std::string text) {};

    void processMessage(std::string text);

};


