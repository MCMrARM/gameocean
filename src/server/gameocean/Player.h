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

/**
 * This is the base class for a player. All network protocols have a class extending this one. All empty methods are
 * most likely implemented in the specialized subclass.
 */
class Player : public Entity, public CommandSender {
    friend class Entity;
    friend class World;
    friend class PlayerInventory;
    friend class PlayerChunkQueueThread;
    friend class PlayerBlockDestroyThread;

protected:
    Server &server;

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
    BlockVariant *miningBlock = nullptr;
    long long miningStarted = -1;
    int miningTime;

    long long lastAttack = 0;

    std::atomic<bool> shouldUpdateChunkQueue;
    std::recursive_mutex chunkArrayMutex;
    std::unordered_map<ChunkPos, ChunkPtr> toSendChunks;
    std::unordered_map<ChunkPos, ChunkPtr> sentChunks;
    std::unordered_map<ChunkPos, ChunkPtr> receivedChunks;
    std::vector<ChunkPos> sendChunksQueue;

    std::set<Entity *> spawnedEntities;

    bool isOp = false;
    std::set<Permission *> permissions;

    std::map<Plugin *, void*> pluginData;

    std::shared_ptr<Container> openedContainer;

    InventoryTransaction transaction;
    long long transactionStart;

    void updateChunkQueue();
    void sendQueuedChunks();
    void updateTeleportState();

    /**
     * This function clears all chunk lists (toSendChunks, sentChunks and receivedChunks) as well as the sendChunksQueue
     * and sets shouldUpdateChunkQueue flag to true.
     */
    virtual void forceResendAllChunks();

    virtual bool sendChunk(int x, int z);
    virtual void receivedChunk(int x, int z);

    virtual void setSpawned();
    virtual void respawn();

    virtual void sendPosition(float x, float y, float z) = 0;

    virtual void spawnEntity(Entity *entity) {
        if (closed)
            return;
        spawnedEntities.insert(entity);
    }
    virtual void despawnEntity(Entity *entity) {
        if (closed)
            return;
        spawnedEntities.erase(entity);
    }
    virtual void updateEntityPos(Entity *entity) {};
    virtual void sendHurtAnimation(Entity *entity) {};

    virtual void sendPlayerArmor(Player *player) {};
    virtual void sendPlayerHeldItem(Player *player) {};

    virtual void sendBlockUpdate(BlockPos bpos) = 0;

    int calculateMiningTime();

    virtual void sendWorldTime(int time, bool stopped) = 0;

    virtual void sendHealth(float hp) = 0;
    virtual void sendHunger(float hunger) = 0;

    virtual void sendDeathStatus() = 0;

    void addTransaction(Inventory &inventory, InventoryTransaction::InventoryKind kind, int slot, ItemInstance to);

    void broadcastArmorChange();
    void broadcastHeldItem();

public:
    Player(Server &server);

    PlayerInventory inventory;

    std::string skin, skinModel;

    /**
     * This function will kick the player out of server with the specified reason.
     */
    virtual void close(std::string reason, bool sendToPlayer);
    virtual void close() {
        close("unknown", true);
    }

    virtual std::string getName() { return name; };
    void setName(std::string name) {
        this->name = name;
    }

    inline bool hasSpawned() { return spawned; };

    static const char *TYPE_NAME;
    virtual const char *getTypeName() { return TYPE_NAME; };

    /**
     * This function will switch the current player's world. It is recommended to use teleport() function instead, to
     * avoid possible MCPE bugs.
     */
    virtual void setWorld(World &world, float x, float y, float z);
    /**
     * This function will change the player's position to the specified coordinates. When teleporting to far distances,
     * it is recommended to use teleport, as it'll first send the chunks and then teleport the player, avoiding some
     * possible MCPE bugs.
     */
    virtual void setPos(float x, float y, float z);
    /**
     * This function will teleport the player to the specified coordinates. First missing chunks will be sent, and after
     * those are received, player will be actually teleported to the specified coordinates.
     */
    void teleport(float x, float y, float z);
    /**
     * This function will teleport the player to the specified coordinates in the specified world. It works similarly
     * to teleport(x, y, z) - it'll first send the chunks, and then after those are received, it'll update the player's
     * position.
     */
    void teleport(World &world, float x, float y, float z);

    /**
     * This function will attempt to move the player to the specified coordinates, while checking for collisions, etc.
     * If the result position is different from the specified, this function will return false.
     */
    bool tryMove(float x, float y, float z);

    /**
     * Checks if the player is walking in fluid or is inside it.
     */
    bool isInFluid();
    /**
     * Checks if the player is fully under fluid.
     */
    bool isUnderFluid();

    virtual void sendMessage(std::string text) {}

    /**
     * This function will process a message as if it was sent by the player. It'll handle commands or broadcast the
     * message to chat if it's not a command.
     */
    void processMessage(std::string text);

    /**
     * This function will send the specified inventory slot to the player. You shouldn't use it unless you have a good
     * reason to do so.
     */
    virtual void sendInventorySlot(int slotId) = 0;
    /**
     * This function will resend the whole inventory to the player. You shouldn't use it unless you have a good reason
     * to do so.
     */
    virtual void sendInventory() = 0;
    virtual void sendArmor() { sendPlayerArmor(this); }
    virtual void sendHeldItem() { sendPlayerHeldItem(this); }

    /**
     * Internal use. Starts mining a block at the specified coordinates.
     */
    virtual void startMining(BlockPos pos);
    /**
     * This function cancels mining the current mined block.
     */
    virtual void cancelMining();
    /**
     * This function finishes mining the current mined block, even if it is not actually finished.
     */
    virtual void finishedMining();
    /**
     * This function returns how much time it takes to mine the currently mined block.
     */
    inline int getMiningTime() { return miningTime; };
    /**
     * This function returns in how much time the player will finish mining the currently mined block.
     */
    int getRemainingMiningTime();

    /**
     * Internal use. This function will attack the specified Entity.
     */
    void attack(Entity &entity);
    virtual void damage(EntityDamageEvent &event);
    /**
     * This function returns the multiplier that includes the armor damage reduction. When you don't have any armor,
     * this function will return 1.f. The return value is between 0.f - 1.f.
     */
    float getArmorReductionMultiplier();

    virtual void setHealth(float hp);

    virtual void kill();

    /**
     * This function checks if the player is an server operator. Server operators gain access to special commands and
     * privileges.
     */
    inline bool isOperator() {
        std::unique_lock<std::recursive_mutex> lock (generalMutex);
        return isOp;
    }
    /**
     * Sets the player to be a server operator or remove the privilege from him.
     */
    void setOperator(bool op);
    bool hasPermission(Permission *perm);
    void grantPermissions(std::set<Permission *> perms, bool children);
    inline void grantPermission(Permission *perm, bool children) {
        grantPermissions({ perm }, children);
    }
    void removePermissions(std::set<Permission *> perms, bool children);
    inline void removePermissions(Permission *perm, bool children) {
        removePermissions({ perm }, children);
    }

    /**
     * This function get the data stored by a plugin for this player. You should check if the return is not null and if
     * it's not, cast it to your own internal type.
     */
    void *getPluginData(Plugin *plugin);
    /**
     * This function sets plugin data for this player. You can cast any pointer to void * and pass it to this function.
     * You can later get the data using the getPluginData() function. This data is session only - it won't be stored
     * after a restart or reconnect.
     */
    void setPluginData(Plugin *plugin, void *data);

    /**
     * This function opens a container (eg. chests) as this player.
     */
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

    /**
     * This function is internal use only.
     */
    virtual void tickPhysics();

    /**
     * This function is internal use only.
     */
    virtual void update();

    void tickHunger();
    void addFoodExhaustion(float amount);
    void setHunger(float points);
    float getHunger();
    void restoreHunger(float hunger, float saturation);

    /**
     * Enables or disables hunger for this player. When disabled, the player won't lose hunger when walking, breaking
     * blocks, etc. but still will be able to regain it by eating food.
     */
    inline void setFoodEnabled(bool enabled) {
        std::unique_lock<std::recursive_mutex> lock (generalMutex);
        hungerDisabled = !enabled;
    }

    /**
     * This function returns the time that it takes to receive a pong reply for a ping request, in ms. If unspecified
     * the function should return -1.
     */
    virtual int getPing() { return -1; }

};


