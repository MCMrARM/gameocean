#pragma once

#include "../raknet/RakNetConnection.h"
class MCPEPlayer;

class MCPEConnection : public RakNetConnection {

protected:
    std::shared_ptr<MCPEPlayer> mcpePlayer = nullptr;

public:
    MCPEConnection(RakNetProtocolServer &server, sockaddr_in addr) : RakNetConnection(server, addr) {
        //
    }

    inline std::shared_ptr<MCPEPlayer> getMCPEPlayer() {
        return mcpePlayer;
    }
    inline void setMCPEPlayer(std::shared_ptr<MCPEPlayer> player) {
        mcpePlayer = std::move(player);
    }
    inline bool hasMCPEPlayer() {
        return mcpePlayer ? true : false;
    }

};


