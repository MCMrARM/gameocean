#include "Server.h"

#include "GameInfo.h"
#include "utils/Config.h"
#include "utils/Logger.h"
#include "protocol/mcpe/MCPEProtocol.h"
#include "world/World.h"

Server::Server() {
    mainWorld = new World("test");
}

void Server::start() {
    Logger::main->info("Main", "%s", (std::string("Game: ") + GameInfo::current->name + " v" + GameInfo::current->version.toString()).c_str());

    Logger::main->info("Main", "Loading server configuration");
    this->loadConfiguation();

    Logger::main->info("Main", "Server name: %s", this->name.c_str());

    Chunk* chunk = new Chunk(0, 0);

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 8; y++) {
                chunk->setBlock(x, y, z, rand() % 3 + 1, rand() % 3);
            }
        }
    }

    mainWorld->setChunk(chunk);
    mainWorld->spawn.y = 10;

    MCPEProtocol protocol (*this);
    protocol.bind(19132);
    protocol.loop();
}

void Server::loadConfiguation() {
    Config c ("config.yml");
    this->name = c.getString("name", "Test Server");
}