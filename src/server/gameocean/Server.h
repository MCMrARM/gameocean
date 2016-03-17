#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <algorithm>
#include <set>
#include "PlayerBlockDestroyThread.h"
#include <gameocean/plugin/PluginManager.h>

class World;
class Player;
class Protocol;

class Server {

protected:
    std::vector<std::shared_ptr<Player>> players;
    std::mutex playersMutex;

    bool stopping = false;

public:
    std::string name;
    int maxPlayers = 100;
    int sendChunksCount = 4;
    int sendChunksDelay = 25; // in ms
    int maxChunkSendCount = 96;
    World* mainWorld;
    std::set<Protocol*> enabledProtocols;
    PlayerBlockDestroyThread playerBlockDestroyThread;
    PluginManager pluginManager;

    Server();

    void start();

    std::vector<std::shared_ptr<Player>> getPlayers() {
        playersMutex.lock();
        std::vector<std::shared_ptr<Player>> playersCopy (players);
        playersMutex.unlock();
        return playersCopy;
    }

    void addPlayer(std::shared_ptr<Player> player) {
        playersMutex.lock();
        players.push_back(player);
        playersMutex.unlock();
    }
    void removePlayer(std::shared_ptr<Player> player) {
        playersMutex.lock();
        players.erase(std::remove(players.begin(), players.end(), player), players.end());
        playersMutex.unlock();
    }
    std::shared_ptr<Player> findPlayer(std::string like);
    void broadcastMessage(std::string msg);

    inline bool isStopping() { return stopping; };
    void stop();

    void reload();

protected:
    void loadConfiguation();

};


