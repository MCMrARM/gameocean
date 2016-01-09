#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <set>
#include <atomic>
#include <memory>
#include <gameocean/item/BlockVariant.h>
#include <gameocean/common.h>
#include <gameocean/entity/Entity.h>
#include "Server.h"
#include <gameocean/world/ChunkPos.h>
#include "command/CommandSender.h"
#include "PlayerInventory.h"
#include <gameocean/world/BlockPos.h>
#include "inventory/transaction/InventoryTransaction.h"

class Server;
class Protocol;
class Chunk;
class PlayerChunkQueueThread;
class PlayerBlockDestroyThread;
class Plugin;
class Container;

class Player : public Entity, public CommandSender {
    friend class Entity;
    friend class World;
    friend class PlayerInventory;
    friend class PlayerChunkQueueThread;
    friend class PlayerBlockDestroyThread;

protected:
    Server& server;

    std::string name;

    std::atomic<bool> spawned;
    std::atomic<bool> teleporting;
    int viewChunks = 94;

    BlockPos miningBlockPos;
    BlockVariant* miningBlock;
    long long miningStarted = -1;
    int miningTime;

    long long lastAttack = 0;

    std::atomic<bool> shouldUpdateChunkQueue;
    std::recursive_mutex chunkArrayMutex;
    std::unordered_map<ChunkPos, Chunk*> sentChunks;
    std::unordered_map<ChunkPos, Chunk*> receivedChunks;
    std::vector<ChunkPos> sendChunksQueue;

    std::set<Entity*> spawnedEntities;

    std::map<Plugin*, void*> pluginData;

    std::shared_ptr<Container> openedContainer;

    InventoryTransaction transaction;
    long long transactionStart;

    void updateChunkQueue();
    void sendQueuedChunks();
    void updateTeleportState();

    virtual bool sendChunk(int x, int z);
    virtual void receivedChunk(int x, int z);

    virtual void setSpawned();
    virtual void respawn();

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
    virtual void sendHurtAnimation(Entity* entity) {};

    virtual void sendPlayerArmor(Player* player) {};
    virtual void sendPlayerHeldItem(Player* player) {};

    virtual void sendBlockUpdate(BlockPos bpos) = 0;

    int calculateMiningTime();

    virtual void sendWorldTime(int time, bool stopped) = 0;

    virtual void sendHealth(float hp) = 0;

    virtual void sendDeathStatus() = 0;

    void addTransaction(Inventory& inventory, InventoryTransaction::InventoryKind kind, int slot, ItemInstance to);

    void broadcastArmorChange();

public:
    Player(Server& server);

    PlayerInventory inventory;

    std::string skin, skinModel;

    virtual void close(std::string reason, bool sendToPlayer);
    virtual void close() {
        close("unknown", true);
    };

    virtual std::string getName() { return name; };
    void setName(std::string name) {
        this->name = name;
    }

    inline bool hasSpawned() { return spawned; };

    static const char* TYPE_NAME;
    virtual const char* getTypeName() { return TYPE_NAME; };

    virtual void setWorld(World& world, float x, float y, float z);
    virtual void setPos(float x, float y, float z);
    void teleport(float x, float y, float z);
    void teleport(World& world, float x, float y, float z);

    bool tryMove(float x, float y, float z);

    virtual void sendMessage(std::string text) {};

    void processMessage(std::string text);

    virtual void sendInventorySlot(int slotId) = 0;
    virtual void sendInventory() = 0;
    virtual void sendArmor() { sendPlayerArmor(this); };
    virtual void sendHeldItem() { sendPlayerHeldItem(this); };

    virtual void startMining(BlockPos pos);
    virtual void cancelMining();
    virtual void finishedMining();
    inline int getMiningTime() { return miningTime; };
    int getRemainingMiningTime();

    void attack(Entity& entity);
    virtual void damage(EntityDamageEvent& event);
    float getArmorReductionMultiplier();

    virtual void setHealth(float hp);

    virtual void kill();

    void* getPluginData(Plugin* plugin);
    void setPluginData(Plugin* plugin, void* data);

    virtual void openContainer(std::shared_ptr<Container> container) {
        openedContainer = container;
    }
    virtual void sendContainerContents() { }
    virtual void closeContainer() {
        openedContainer.reset();
        transaction.revert();
        transaction.reset();
    }
    inline std::shared_ptr<Container> getOpenedContainer() {
        return openedContainer;
    }

    virtual void tickPhysics();

};


