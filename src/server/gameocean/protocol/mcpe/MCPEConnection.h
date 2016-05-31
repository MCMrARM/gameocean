#pragma once

#include "../raknet/RakNetConnection.h"
class MCPEPlayer;

class MCPEConnection : public RakNetConnection {

protected:
    MCPEPlayer *mcpePlayer = nullptr;

public:
    MCPEConnection(RakNetProtocolServer &server, sockaddr_in addr) : RakNetConnection(server, addr) {
        //
    }

    inline MCPEPlayer *getMCPEPlayer() {
        return mcpePlayer;
    }
    inline void setMCPEPlayer(MCPEPlayer *player) {
        mcpePlayer = player;
    }
    inline bool hasMCPEPlayer() {
        return (mcpePlayer != nullptr);
    }

};


