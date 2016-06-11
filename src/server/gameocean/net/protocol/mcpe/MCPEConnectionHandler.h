#pragma once

#include "../raknet/RakNetConnectionHandler.h"

class MCPEConnectionHandler : public RakNetConnectionHandler {

public:
    virtual void connected(Connection &connection);

    virtual void disconnected(Connection &connection, Connection::DisconnectReason reason, std::string textReason);

    virtual void onPacketDelivered(RakNetConnection &connection, int ackId);

};;