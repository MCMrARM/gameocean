#pragma once

#include "MCPEPacketBatchThread.h"
#include "../raknet/RakNetProtocolServer.h"
#include "MCPEConnection.h"
#include "MCPEConnectionHandler.h"

class MCPEProtocolServer : public RakNetProtocolServer {

protected:
    MCPEPacketBatchThread batchThread;
    MCPEConnectionHandler connectionHandler;
    std::set<std::shared_ptr<MCPEPlayer>> players;
    std::mutex playersMutex;

public:
    int packetBatchDelay = 50; // in ms

    MCPEProtocolServer(Protocol& protocol) : RakNetProtocolServer(protocol), batchThread(*this) {
        port = 19132;
        setRakNetHandler(&connectionHandler);
    }

    void updateServerName();

    virtual void loop() {
        updateServerName();
        batchThread.start();
        RakNetProtocolServer::loop();
        batchThread.stop();
    }

    virtual void setOption(std::string key, std::string value) {
        if (key == "batch-rate") {
            packetBatchDelay = StringUtils::asInt(value, packetBatchDelay);
        } else {
            RakNetProtocolServer::setOption(key, value);
        }
    }

    virtual std::shared_ptr<RakNetConnection> createRakNetConnection(sockaddr_in addr) {
        return std::shared_ptr<RakNetConnection>(new MCPEConnection(*this, addr));
    }

    std::set<std::shared_ptr<MCPEPlayer>> getPlayers() {
        std::lock_guard<std::mutex> lock (playersMutex);
        return players;
    }
    void addPlayer(std::shared_ptr<MCPEPlayer> player) {
        std::lock_guard<std::mutex> lock (playersMutex);
        players.insert(player);
    }


};


