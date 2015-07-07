#pragma once

#include "Entity.h"
#include "Server.h"

class Server;
class Protocol;

class Player : public Entity {

protected:
    Server& server;

public:
    Player(Server& server) : Entity(server.mainWorld), server(server) {};

};


