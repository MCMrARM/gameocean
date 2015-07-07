#pragma once

#include <string>

class World;

class Server {

public:
    std::string name;
    int maxPlayers = 100;
    World* mainWorld;

    Server();

    void start();

protected:
    void loadConfiguation();

};


