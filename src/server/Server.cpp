#include "Server.h"

#include <iostream>
#include <utils/ResourceManager.h>
#include "utils/Logger.h"
#include "game/Item.h"
#include "game/Block.h"
#include "GameInfo.h"
#include "utils/Config.h"
#include "protocol/Protocol.h"
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

    FileResourceManager* resManager = new FileResourceManager("assets/", ".");
    ResourceManager::instance = resManager;

    Logger::main->info("Main", "Loading server configuration");
    Command::registerDefaultCommands(*this);
    Item::registerItems();
    Block::registerBlocks();
    Protocol::registerDefaultProtocols(*this);

    this->loadConfiguation();

    Logger::main->info("Main", "Server name: %s", this->name.c_str());

    MCAnvilProvider* provider = new MCAnvilProvider(*mainWorld);
    mainWorld->setWorldProvider(provider);

    mainWorld->spawn.x = 10;
    mainWorld->spawn.y = 100;
    mainWorld->spawn.z = 10;
    mainWorld->loadSpawnTerrain();

    PlayerChunkQueueThread chunkQueueThread (*this);
    chunkQueueThread.start();

    for (Protocol* protocol : enabledProtocols) {
        protocol->start();
    }

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

    delete resManager;
}

void Server::loadConfiguation() {
    Config c (*ResourceManager::instance->openDataFile("config.yml", std::ios_base::in));
    name = c.getString("name", "Test Server");
    maxPlayers = c.getInt("max-players", maxPlayers);
    std::shared_ptr<ContainerConfigNode> chunkSending = c.getContainer("chunk-sending");
    if (chunkSending != null) {
        sendChunksDelay = chunkSending->getInt("tick-rate", sendChunksDelay);
        sendChunksCount = chunkSending->getInt("per-tick", sendChunksCount);
    }
    std::shared_ptr<ContainerConfigNode> protocols = c.getContainer("protocols");
    if (protocols != null) {
        for (auto const& p : protocols->val) {
            if (p.second->type != ConfigNode::Type::CONTAINER)
                continue;

            Protocol* protocol = Protocol::getProtocol(p.first);
            if (protocol == null) {
                Logger::main->warn("Config", "Unknown protocol: %s", p.first.c_str());
                continue;
            }
            bool enabled = p.second->getBool("enabled", true);
            if (enabled) {
                enabledProtocols.insert(protocol);
            }
            for (auto const& opt : std::static_pointer_cast<ContainerConfigNode>(p.second)->val) {
                protocol->setOption(opt.first, *opt.second);
            }
        }
    }
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