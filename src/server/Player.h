#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <set>
#include <atomic>
#include "common.h"
#include "Entity.h"
#include "Server.h"
#include "world/ChunkPos.h"
#include "command/CommandSender.h"
#include "protocol/mcpe/MCPEPacket.h"
#include "PlayerInventory.h"

class Server;
class Protocol;
class Chunk;

class Player : public Entity, public CommandSender {
    friend class Entity;

protected:
    Server& server;

    std::string name;

    bool spawned = false;
    bool teleporting = false;
    int viewChunks = 94;

    std::atomic<bool> shouldUpdateChunkQueue;
    std::recursive_mutex chunkArrayMutex;
    std::unordered_map<ChunkPos, Chunk*> sentChunks;
    std::unordered_map<ChunkPos, Chunk*> receivedChunks;
    std::vector<ChunkPos> sendChunksQueue;

    std::set<Entity*> spawnedEntities;

    virtual bool sendChunk(int x, int z);
    virtual void receivedChunk(int x, int z);

    virtual void setSpawned();

    virtual void sendPosition(float x, float y, float z) = 0;

    virtual void spawnEntity(Entity* entity) {
        if (closed)
            return;
        spawnedEntities.insert(entity);
    };
    virtual void despawnEntity(Entity* entity) {
        if (closed)
            return;
        spawnedEntities.erase(entity);
    };
    virtual void updateEntityPos(Entity* entity) {};

public:
    Player(Server& server) : Entity(*server.mainWorld), server(server), shouldUpdateChunkQueue(false), inventory(*this, 36) {};

    PlayerInventory inventory;

    virtual void close(std::string reason, bool sendToPlayer);

    virtual std::string getName() { return name; };
    void setName(std::string name) {
        this->name = name;
    }

    inline bool hasSpawned() { return spawned; };

    static const std::string TYPE_NAME;
    virtual std::string getTypeName() { return TYPE_NAME; };

    virtual void setPos(float x, float y, float z);
    void teleport(float x, float y, float z);

    bool tryMove(float x, float y, float z);

    void updateChunkQueue();
    void sendQueuedChunks();
    void updateTeleportState();

    virtual void sendMessage(std::string text) {};

    void processMessage(std::string text);

    virtual void sendInventorySlot(int slotId) = 0;
    virtual void sendInventory() = 0;
    virtual void sendHeldItem() = 0;

};


