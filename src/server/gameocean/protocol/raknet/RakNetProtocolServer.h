#pragma once

#include <string>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <gameocean/net/protocol/ProtocolServer.h>
#include <gameocean/net/ServerSocket.h>
#include "RakNetConnection.h"
#include "RakNetConnectionHandler.h"

class RakNetProtocolServer : public ProtocolServer {

protected:
    struct SockaddrHash {
        size_t operator()(const sockaddr &addr) const {
            return std::_Hash_impl::hash(&addr, sizeof(addr));
        }
    };
    struct SockaddrComparator {
        constexpr bool operator()(const sockaddr &addr1, const sockaddr &addr2) const {
            return memcmp(&addr1, &addr2, sizeof(sockaddr)) == 0;
        }
    };

    RakNetConnectionHandler *rakNetHandler = nullptr;

    ServerSocket socket;
    std::string serverName;
    std::mutex serverNameMutex;
    long long serverId;
    std::unordered_map<sockaddr, std::shared_ptr<RakNetConnection>, SockaddrHash, SockaddrComparator> clients;

public:
    RakNetProtocolServer(Protocol& protocol);

    inline void setRakNetHandler(RakNetConnectionHandler *handler) {
        setHandler(handler);
        rakNetHandler = handler;
    }

    void setBroadcastedServerName(std::string serverName) {
        std::lock_guard<std::mutex> lock(serverNameMutex);
        this->serverName = serverName;
    }

    inline ServerSocket &getSocket() {
        return socket;
    }

    virtual void loop();
    virtual Connection *handleConnection() {
        return nullptr;
    }

    virtual std::shared_ptr<RakNetConnection> createRakNetConnection(sockaddr_in addr) {
        return std::shared_ptr<RakNetConnection>(new RakNetConnection(*this, addr));
    }

    void removeConnection(RakNetConnection &connection);

};


