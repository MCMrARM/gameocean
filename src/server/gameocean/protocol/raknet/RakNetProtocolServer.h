#pragma once

#include <string>
#include <mutex>
#include <gameocean/net/protocol/ProtocolServer.h>
#include <gameocean/net/ServerSocket.h>

class RakNetProtocolServer : public ProtocolServer {

protected:
    ServerSocket socket;
    std::string serverName;
    std::mutex serverNameMutex;
    long long serverId;

public:
    RakNetProtocolServer(Protocol& protocol);

    void setBroadcastedServerName(std::string serverName) {
        std::lock_guard<std::mutex> lock(serverNameMutex);
        this->serverName = serverName;
    }

    virtual void loop();
    virtual Connection *handleConnection() {
        return nullptr;
    }

};


