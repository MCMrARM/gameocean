#include "Server.h"

#include "GameInfo.h"
#include "utils/Config.h"
#include "utils/Logger.h"
#include "protocol/mcpe/MCPEProtocol.h"

void Server::start() {
    Logger::main->info("Main", "%s", (std::string("Game: ") + GameInfo::current->name + " v" + GameInfo::current->version.toString()).c_str());

    Logger::main->info("Main", "Loading server configuration");
    this->loadConfiguation();

    Logger::main->info("Main", "Server name: %s", this->name.c_str());

    MCPEProtocol protocol (*this);
    protocol.bind(19132);
    protocol.loop();
}

void Server::loadConfiguation() {
    Config c ("config.yml");
    this->name = c.getString("name", "Test Server");
}