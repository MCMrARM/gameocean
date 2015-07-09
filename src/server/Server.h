#pragma once

#include <string>
#include <vector>
#include <mutex>

class World;
class Player;

class Server {

protected:
    std::vector<Player*> players;
    std::mutex playersMutex;

public:
    std::string name;
    int maxPlayers = 100;
    int sendChunksCount = 4;
    int sendChunksDelay = 25; // in ms
    World* mainWorld;

    Server();

    void start();

    std::vector<Player*> getPlayers() {
        playersMutex.lock();
        std::vector<Player*> playersCopy (players);
        playersMutex.unlock();
        return playersCopy;
    }

    void addPlayer(Player* player) {
        playersMutex.lock();
        players.push_back(player);
        playersMutex.unlock();
    }
    void removePlayer(Player* player) {
        playersMutex.lock();
        players.erase(std::remove(players.begin(), players.end(), player), players.end());
        playersMutex.unlock();
    }

protected:
    void loadConfiguation();

};


