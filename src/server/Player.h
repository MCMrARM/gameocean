#pragma once

class Server;
class Protocol;

class Player {

protected:
    Server& server;

public:
    Player(Server& server) : server(server) {};

};


