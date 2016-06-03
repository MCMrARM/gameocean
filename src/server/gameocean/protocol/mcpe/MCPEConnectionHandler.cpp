#include "MCPEConnectionHandler.h"
#include "MCPEConnection.h"
#include "MCPEProtocol.h"
#include "MCPEPlayer.h"

void MCPEConnectionHandler::connected(Connection &connection) {
    MCPEConnection &mcpeConnection = (MCPEConnection &) connection;
    Server &server = *mcpeConnection.getServer().getServer();
    std::shared_ptr<MCPEPlayer> player(new MCPEPlayer(server, (MCPEProtocol &) connection.getProtocol(), mcpeConnection));
    mcpeConnection.setMCPEPlayer(player);
    server.addPlayer(player);
    ((MCPEProtocolServer &) mcpeConnection.getServer()).addPlayer(player);
}