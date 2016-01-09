#pragma once

#include <deque>
#include <memory>
#include <functional>
#include "../../Player.h"
#include "MCPEProtocol.h"
#include "MCPEPacket.h"
#include <RakNet/RakNetTypes.h>
#include <gameocean/world/ChunkPos.h>

class MCPEPacket;
class MCPEPacketBatchThread;
class Chunk;

class MCPEPlayer : public Player {

protected:
    friend class MCPEPacketBatchThread;
    friend class MCPELoginPacket;
    friend class MCPEMovePacket;
    friend class MCPEInteractPacket;
    friend class MCPEPlayerActionPacket;
    friend class MCPEContainerSetSlotPacket;

    int hotbarSlots[9];
    int hotbarSlot = 0;

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
    virtual void sendHurtAnimation(Entity* entity);

    virtual void sendPlayerHeldItem(Player* player);
    virtual void sendPlayerArmor(Player* player);

    virtual void sendBlockUpdate(BlockPos bpos);
    virtual void sendWorldTime(int time, bool stopped);

    virtual void sendHealth(float hp);
    virtual void sendDeathStatus();

public:
    MCPEPlayer(Server& server, MCPEProtocol& protocol, RakNet::RakNetGUID guid, RakNet::SystemAddress address) : Player(server), protocol(protocol), guid(guid), address(address) {
        for (int i = 0; i < 9; i++)
            hotbarSlots[i] = i;
    };
    virtual ~MCPEPlayer() {
        for (QueuedPacket& pk : packetQueue) {
            delete pk.pk;
        }
    };

    virtual void close(std::string reason, bool sendToPlayer);

    inline RakNet::SystemAddress& getAddress() { return address; };

    int directPacket(MCPEPacket* packet);
    int writePacket(std::unique_ptr<MCPEPacket> packet, bool batch);
    inline int writePacket(std::unique_ptr<MCPEPacket> packet) { return writePacket(std::move(packet), packet->priority); };
    void batchPacketCallback(std::unique_ptr<MCPEPacket> packet, QueuedPacketCallback&& sentCallback);
    void receivedACK(int packetId);

    virtual void sendMessage(std::string text);

    virtual void sendInventorySlot(int slotId);
    virtual void sendInventory();

    void linkHeldItem(int hotbarSlot, int inventorySlot);

    virtual void openContainer(std::shared_ptr<Container> container);
    virtual void sendContainerContents();
    virtual void closeContainer();

    virtual void setMotion(Vector3D motion);

};


