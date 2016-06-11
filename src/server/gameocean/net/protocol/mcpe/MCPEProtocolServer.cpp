#include "MCPEProtocolServer.h"
#include "MCPEProtocol.h"
#include <sstream>
#include <gameocean/Server.h>

void MCPEProtocolServer::updateServerName() {
    if (server == nullptr)
        return;
    std::stringstream ss;
    ss << "MCPE;" << server->name << ";" << MCPEProtocol::CURRENT_VERSION << ";" << MCPEProtocol::CURRENT_VERSION_STRING <<
            ";" << server->getPlayerCount() << ";" << server->maxPlayers << ";" << serverId << ";";
    setBroadcastedServerName(ss.str());
}