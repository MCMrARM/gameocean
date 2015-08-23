#include "Server.h"

#include "utils/Logger.h"
#include "GameInfo.h"
#include "utils/Config.h"
#include "protocol/mcpe/MCPEProtocol.h"
#include "world/World.h"
#include "world/mcanvil/MCAnvilProvider.h"
#include "game/Item.h"
#include "command/Command.h"
#include "PlayerChunkQueueThread.h"
#include "utils/BinaryStream.h"
#include "utils/NBT.h"

Server::Server() {
    mainWorld = new World("test");
}

void Server::start() {
    Logger::main->info("Main", "%s", (std::string("Game: ") + GameInfo::current->name + " v" + GameInfo::current->version.toString()).c_str());

    Logger::main->info("Main", "Loading server configuration");
    this->loadConfiguation();

    Logger::main->info("Main", "Server name: %s", this->name.c_str());

    Command::registerDefaultCommands();
    Item::registerItems();

    MCAnvilProvider* provider = new MCAnvilProvider(*mainWorld);
    mainWorld->setWorldProvider(provider);

    mainWorld->spawn.x = 10;
    mainWorld->spawn.y = 100;
    mainWorld->spawn.z = 10;
    mainWorld->loadSpawnTerrain();

    PlayerChunkQueueThread chunkQueueThread (*this);
    chunkQueueThread.start();

    MCPEProtocol protocol (*this);
    protocol.bind(19132);
    protocol.loop();
}

void Server::loadConfiguation() {
    Config c ("config.yml");
    this->name = c.getString("name", "Test Server");
}

void Server::broadcastMessage(std::string msg) {
    playersMutex.lock();
    for (Player* p : players) {
        p->sendMessage(msg);
    }
    playersMutex.unlock();
}