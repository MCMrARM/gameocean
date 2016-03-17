#pragma once

#include <map>
#include "../Protocol.h"
#include "MCPEPacketBatchThread.h"
#include <RakNet/RakNetTypes.h>

namespace RakNet {
    class Packet;
    class RakPeerInterface;
    class BitStream;
};

class MCPEPacket;
class MCPEPlayer;

class MCPEProtocol : public Protocol {

protected:
    RakNet::RakPeerInterface* peer;
    std::map<RakNet::RakNetGUID, MCPEPlayer*> players;
    std::mutex playersMutex;
    MCPEPacketBatchThread batchThread;

public:
    MCPEProtocol(Server& server) : Protocol(server), batchThread(*this) {
        port = 19132;
    };

    virtual std::string getName() { return "MCPE"; };

    static const int CURRENT_VERSION = 45;
    int packetBatchDelay = 50; // in ms

    inline RakNet::RakPeerInterface* getPeer() { return peer; };

    virtual void start();
    virtual void stop();

    virtual void loop();
    virtual void processPacket(RakNet::Packet* packet);

    std::map<RakNet::RakNetGUID, MCPEPlayer*> getPlayers() {
        playersMutex.lock();
        std::map<RakNet::RakNetGUID, MCPEPlayer*> ret (players);
        playersMutex.unlock();
        return ret;
    };

    virtual void setOption(std::string key, std::string value) {
        if (key == "batch-rate") {
            packetBatchDelay = StringUtils::asInt(value, packetBatchDelay);
        } else {
            Protocol::setOption(key, value);
        }
    };

};