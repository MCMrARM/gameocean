#pragma once

#include <gameocean/protocol/raknet/RakNetConnectionHandler.h>

class MCPEConnectionHandler : public RakNetConnectionHandler {

public:
    virtual void connected(Connection &connection);

    virtual void onPacketDelivered(RakNetConnection &connection, int ackId);

};;