#pragma once

#include <string>

class Server {

public:
    std::string name;
    int maxPlayers = 100;

    void start();

protected:
    void loadConfiguation();

};


