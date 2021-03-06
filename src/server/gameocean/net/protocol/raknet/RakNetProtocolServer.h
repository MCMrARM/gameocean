#pragma once

#include <string>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <gameocean/net/protocol/ProtocolServer.h>
#include <gameocean/net/ServerSocket.h>
#include "RakNetConnection.h"
#include "RakNetConnectionHandler.h"
#include "RakNetResendThread.h"
#include "RakNetPingThread.h"

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

    typedef std::unordered_map<sockaddr, std::shared_ptr<RakNetConnection>, SockaddrHash, SockaddrComparator> ClientMap;

    RakNetConnectionHandler *rakNetHandler = nullptr;

    RakNetResendThread resendThread;
    RakNetPingThread pingThread;

    ServerSocket socket;
    std::string serverName;
    std::mutex serverNameMutex;
    long long serverId;
    std::mutex clientsMutex;
    ClientMap clients;

public:
    int timeoutTime = 10000; // in ms
    int timeoutResendDelay = 500; // in ms
    int timeoutAttemptCount = 5;

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

    inline ClientMap getConnections() {
        std::lock_guard<std::mutex> lock(clientsMutex);
        return clients;
    }

    virtual void setOption(std::string key, std::string value) {
        if (key == "resend-interval") {
            resendThread.resendDelay = StringUtils::asInt(value, resendThread.resendDelay);
        } else if (key == "ping-interval") {
            pingThread.pingInterval = StringUtils::asInt(value, pingThread.pingInterval);
        } else if (key == "timeout-start") {
            // when player should start timeouting - at this stage the server will try to get a reply from client,
            // resending the timeout packet every timeout-resend ms.
            pingThread.pingInterval = StringUtils::asInt(value, pingThread.pingInterval);
        } else if (key == "timeout-check-delay") {
            // delay between connection check packets sent (they get sent if a client does not reply to any ping within
            // the value specified in timeout-start)
            pingThread.pingInterval = StringUtils::asInt(value, pingThread.pingInterval);
        } else if (key == "timeout-check-attempts") {
            // how many times the client has not to reply to a timeout check packet before it gets disconnected
            pingThread.pingInterval = StringUtils::asInt(value, pingThread.pingInterval);
        } else {
            ProtocolServer::setOption(key, value);
        }
    }

};


