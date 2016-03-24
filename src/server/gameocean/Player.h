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
class Permission;

class Player : public Entity, public CommandSender {
    friend class Entity;
    friend class World;
    friend class PlayerInventory;
    friend class PlayerChunkQueueThread;
    friend class PlayerBlockDestroyThread;

protected:
    Server& server;

    std::string name;

    bool hungerDisabled = false;
    float hunger = 20.f;
    float foodSaturation = 0.f;
    float foodExhaustion = 0.f;
    long long foodLastHpChange = -1;

    std::atomic<bool> spawned;
    std::atomic<bool> teleporting;
    int viewChunks = 94;

    bool isSprinting = false;

    BlockPos miningBlockPos;
    BlockVariant* miningBlock = nullptr;
    long long miningStarted = -1;
    int miningTime;

    long long lastAttack = 0;

    std::atomic<bool> shouldUpdateChunkQueue;
    std::recursive_mutex chunkArrayMutex;
    std::unordered_map<ChunkPos, ChunkPtr> toSendChunks;
    std::unordered_map<ChunkPos, ChunkPtr> sentChunks;
    std::unordered_map<ChunkPos, ChunkPtr> receivedChunks;
    std::vector<ChunkPos> sendChunksQueue;

    std::set<Entity*> spawnedEntities;

    bool isOp = false;
    std::set<Permission*> permissions;

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
    virtual void sendHunger(float hunger) = 0;

    virtual void sendDeathStatus() = 0;

    void addTransaction(Inventory& inventory, InventoryTransaction::InventoryKind kind, int slot, ItemInstance to);

    void broadcastArmorChange();
    void broadcastHeldItem();

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

    bool isInFluid();
    bool isUnderFluid();

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

    inline bool isOperator() {
        std::unique_lock<std::recursive_mutex> lock (generalMutex);
        return isOp;
    }
    void setOperator(bool op);
    bool hasPermission(Permission* perm);
    void grantPermissions(std::set<Permission*> perms, bool children);
    inline void grantPermission(Permission* perm, bool children) {
        grantPermissions({ perm }, children);
    }
    void removePermissions(std::set<Permission*> perms, bool children);
    inline void removePermissions(Permission* perm, bool children) {
        removePermissions({ perm }, children);
    }

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

    virtual void update();

    void tickHunger();
    void addFoodExhaustion(float amount);
    void setHunger(float points);
    float getHunger();
    void restoreHunger(float hunger, float saturation);

    inline bool setFoodEnabled(bool enabled) {
        std::unique_lock<std::recursive_mutex> lock (generalMutex);
        hungerDisabled = !enabled;
    }

};


