#pragma once

#include <gameocean/net/ConnectionHandler.h>
class RakNetConnection;

class RakNetConnectionHandler : public ConnectionHandler {

public:
    /**
     * This method is called when a packet that required an ACK Receipt was successfully delivered.
     */
    virtual void onPacketDelivered(RakNetConnection &connection, int ackId) { }

    /**
     * This method is called when an unreliable packet that required an ACK Receipt was lost.
     */
    virtual void onPacketLost(RakNetConnection &connection, int ackId) { }

};