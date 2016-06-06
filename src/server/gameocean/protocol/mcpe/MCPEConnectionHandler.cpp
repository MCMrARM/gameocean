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

void MCPEConnectionHandler::disconnected(Connection &connection, Connection::DisconnectReason reason,
                                         std::string textReason) {
    MCPEConnection &mcpeConnection = (MCPEConnection &) connection;
    auto pl = mcpeConnection.getMCPEPlayer();
    mcpeConnection.setMCPEPlayer(std::shared_ptr<MCPEPlayer>());
    if (pl) {
        mcpeConnection.getServer().getServer()->removePlayer(pl);
        ((MCPEProtocolServer &) mcpeConnection.getServer()).removePlayer(pl);
        pl->close((textReason.length() == 0 ? (reason == Connection::DisconnectReason::TIMEOUT ? "Timeout" : "Disconnect") : textReason), false);
    }
}

void MCPEConnectionHandler::onPacketDelivered(RakNetConnection &connection, int ackId) {
    MCPEConnection &mcpeConnection = (MCPEConnection &) connection;
    auto pl = mcpeConnection.getMCPEPlayer();
    if (pl)
        pl->receivedACK(ackId);
}