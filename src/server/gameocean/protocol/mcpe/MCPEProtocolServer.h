#pragma once

#include "MCPEPacketBatchThread.h"
#include "../raknet/RakNetProtocolServer.h"
#include "MCPEConnection.h"

class MCPEProtocolServer : public RakNetProtocolServer {

protected:
    MCPEPacketBatchThread batchThread;

public:
    int packetBatchDelay = 50; // in ms

    MCPEProtocolServer(Protocol& protocol) : RakNetProtocolServer(protocol), batchThread(*this) {
        port = 19132;
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


};


