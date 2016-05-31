#pragma once

#include <map>
#include <gameocean/net/Connection.h>
#include <netinet/in.h>
#include "RakNetReliability.h"
#include "RakNetCompound.h"
class RakNetProtocolServer;

class RakNetConnection : public Connection {

protected:
    RakNetProtocolServer &server;
    sockaddr_in addr;
    int mtu = 1492;
    int sendFrameIndex = 0;
    int sendReliableIndex = 0;
    int sendSequencedIndex = 0;
    std::map<int, RakNetCompound> receiveCompounds;

public:
    RakNetConnection(RakNetProtocolServer &server, sockaddr_in addr);
    virtual ~RakNetConnection() {
        close();
    }

    void setPrefferedMTU(int mtu);
    inline sockaddr_in const &getSockAddr() { return addr; }
    inline int getMTU() { return mtu; }

    virtual void close();

    /**
     * Deprecated. Please use sendRaw or send(Packet &, RakNetReliability) instead
     */
    virtual void send(Packet &packet) { sendRaw(packet); }
    /**
     * This function sends the packet with the specified reliability. If the packet is supposed to be Reliable then
     * this function will return the corresponding packet id. Otherwise this function should return 0.
     */
    virtual int send(Packet &packet, RakNetReliability reliability);
    void sendRaw(Packet &packet);
    virtual bool readAndHandlePacket() {
        return false; // this doesn't work this way with udp
    }

    void handleFragmentedPacket(std::vector<char> data, int compoundSize, int compoundId, int index);

};


