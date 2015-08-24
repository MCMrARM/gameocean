#include "Server.h"

#include <iostream>
#include "utils/Logger.h"
#include "game/Item.h"
#include "game/Block.h"
#include "GameInfo.h"
#include "utils/Config.h"
#include "protocol/mcpe/MCPEProtocol.h"
#include "world/World.h"
#include "world/mcanvil/MCAnvilProvider.h"
#include "command/Command.h"
#include "PlayerChunkQueueThread.h"
#include "utils/StringUtils.h"

Server::Server() {
    mainWorld = new World("test");
}

void Server::start() {
    Logger::main->info("Main", "%s", (std::string("Game: ") + GameInfo::current->name + " v" + GameInfo::current->version.toString()).c_str());

    Logger::main->info("Main", "Loading server configuration");
    this->loadConfiguation();

    Logger::main->info("Main", "Server name: %s", this->name.c_str());

    Command::registerDefaultCommands(*this);
    Item::registerItems();
    Block::registerBlocks();

    MCAnvilProvider* provider = new MCAnvilProvider(*mainWorld);
    mainWorld->setWorldProvider(provider);

    mainWorld->spawn.x = 10;
    mainWorld->spawn.y = 100;
    mainWorld->spawn.z = 10;
    mainWorld->loadSpawnTerrain();

    PlayerChunkQueueThread chunkQueueThread (*this);
    chunkQueueThread.start();

    MCPEProtocol protocol (*this);
    protocol.start(19132);

    timeval tv = { 0, 50000 };
    fd_set fds;

    ServerCommandSender commandSender;
    while (true) {
        if (stopping)
            break;

        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) <= 0)
            continue;

        std::string command;
        std::cin >> command;

        std::vector<std::string> v = StringUtils::split(command, " ");
        if (v.size() <= 0)
            continue;
        Command* c = Command::getCommand(v[0]);
        if (c == null) {
            std::cout << "Unknown command: " << v[0] << std::endl;
            continue;
        }
        c->process(commandSender, v);
    }

    Logger::main->trace("Main", "Stopping...");
    for (Thread* t : Thread::threads) {
        t->stop();
    }
}

void Server::loadConfiguation() {
    Config c ("config.yml");
    this->name = c.getString("name", "Test Server");
}

Player* Server::findPlayer(std::string like) {
    Player* rp = null;
    int m = 0;

    playersMutex.lock();
    for (Player* p : players) {
        std::string n = p->getName();
        if (n == like) {
            return p;
        }

        int i = StringUtils::compare(like, n);
        if (i > m) {
            rp = p;
            m = i;
        }
    }
    playersMutex.unlock();

    return rp;
}

void Server::broadcastMessage(std::string msg) {
    Logger::main->info("Chat", "%s", msg.c_str());

    playersMutex.lock();
    for (Player* p : players) {
        p->sendMessage(msg);
    }
    playersMutex.unlock();
}

void Server::stop() {
    stopping = true;
}