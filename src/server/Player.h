#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include "Entity.h"
#include "Server.h"
#include "world/ChunkPos.h"

class Server;
class Protocol;
class Chunk;

class Player : public Entity {

protected:
    Server& server;

    bool spawned = false;
    bool teleporting = false;
    int viewChunks = 94;

    std::atomic<bool> shouldUpdateChunkQueue;
    std::mutex chunkArrayMutex;
    std::unordered_map<ChunkPos, Chunk*> sentChunks;
    std::unordered_map<ChunkPos, Chunk*> receivedChunks;
    std::vector<ChunkPos> sendChunksQueue;

    virtual bool sendChunk(int x, int z);
    virtual void receivedChunk(int x, int z);

public:
    Player(Server& server) : Entity(*server.mainWorld), server(server), shouldUpdateChunkQueue(false) {};

    virtual void setPos(float x, float y, float z);
    void teleport(float x, float y, float z);

    bool tryMove(float x, float y, float z);

    void updateChunkQueue();
    void sendQueuedChunks();

    virtual void sendMessage(std::string text) {};

};


