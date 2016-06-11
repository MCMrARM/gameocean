#pragma once

#include <map>
#include <unordered_map>
#include <chrono>
#include <gameocean/net/Connection.h>
#include <netinet/in.h>
#include <gameocean/utils/BinaryStream.h>
#include "RakNetReliability.h"
#include "RakNetCompound.h"
#include "RakNetConnectionHandler.h"
class RakNetProtocolServer;

class RakNetConnection : public Connection {

protected:
    friend class RakNetACKPacket;
    friend class RakNetNAKPacket;
    friend class RakNetClientHandshakePacket;
    friend class RakNetPongPacket;

    struct SendFrameCompound {
        int id = -1;
        int frameRefs = 0;
        int frameCount;
        int orderIndex = -1;
        char orderChannel = 0;
        int sequencedIndex = -1;
        int ackReceiptId = -1;
    };
    struct SendFrame {
        std::vector<char> data;
        RakNetReliability reliability;
        int reliableIndex = -1;
        int seqencedIndex = -1;
        int orderIndex = -1;
        char orderChannel = 0;
        SendFrameCompound *compound = nullptr;
        int compoundIndex = -1;
        int reliableACKReceiptId = -1;
    };
    struct PacketMeta {
        int unreliableAckReceiptId = -1;
        int reliableFrameId = -1;
    };

    int pingTime = -1;

    std::mutex packetTimesMutex;
    std::chrono::steady_clock::time_point lastPacketTime; // what was the time when the last packet (of any time) was received
    std::chrono::steady_clock::time_point lastTimeoutCheckPacketSentTime; // when was the last timeout check packet sent
    int timeoutPacketAttempts = 0; // when the client is most likely timeouted, this variable specifies how many times a test packet was already sent

    std::recursive_mutex sendReliableMutex;
    std::map<int, SendFrame> sendReliableFrames;
    std::map<int, PacketMeta> sentPackets;
    std::unordered_map<int, std::chrono::steady_clock::time_point> sendReliableQueue;
    bool sendReliableQueueSorted = false;

    int sendFrame(SendFrame &frame);

    void onPacketDelivered(int pkId);
    void onPacketLost(int pkId);
    void onReliableFrameDelivered(int frameId);
    void queueReliablePacketResend(int frameId, std::chrono::milliseconds resendTime);
    void dequeueReliablePacketResend(int frameId);

    RakNetProtocolServer &server;
    RakNetConnectionHandler *rakNetConnectionHandler = nullptr;
    sockaddr_in addr;
    unsigned int mtu = 1492;
    int ackReceiptId = 1;
    int sendFrameIndex = 0;
    int sendReliableIndex = 0;
    int sendSequencedIndex[256];
    int sendOrderIndex[256];
    int sendCompoundIndex = 0;
    int receiveSequencedIndex[256];
    int receiveOrderIndex[256];
    int receiveReliableIndexMin = -1;
    std::set<int> receiveReliableIndexes;
    std::map<int, RakNetCompound> receiveCompounds;
    std::map<std::pair<byte, int>, std::vector<char>> receiveOrderedQueue; // pair<channelId, orderId>, data

public:
    RakNetConnection(RakNetProtocolServer &server, sockaddr_in addr);
    virtual ~RakNetConnection() {
        doClose();
    }

    inline RakNetProtocolServer &getServer() {
        return server;
    }

    void setPrefferedMTU(int mtu);
    inline sockaddr_in const &getSockAddr() { return addr; }
    inline int getMTU() { return mtu; }

    virtual void doClose();

    /**
     * Deprecated. Please use sendRaw or send(Packet &, RakNetReliability) instead
     */
    virtual void send(Packet &packet) { sendRaw(packet); }
    /**
     * This function sends the packet with the specified reliability. If the packet is supposed to be Reliable then
     * this function will return the corresponding packet id. Otherwise this function should return 0.
     */
    virtual int send(Packet &packet, RakNetReliability reliability, int orderChannel = 0);
    void sendRaw(Packet &packet);
    virtual bool readAndHandlePacket() {
        return false; // this doesn't work this way with udp
    }

    void setRakNetHandler(RakNetConnectionHandler *handler) {
        setHandler(handler);
        rakNetConnectionHandler = handler;
    }
    inline RakNetConnectionHandler *getRakNetHandler() {
        return rakNetConnectionHandler;
    }

    bool handleReliableIndex(int index) {
        if (index <= receiveReliableIndexMin) {
            return false;
        }
        if (index == receiveReliableIndexMin + 1) {
            receiveReliableIndexMin++;
            std::set<int>::iterator it = receiveReliableIndexes.find(receiveReliableIndexMin + 1);
            while (it != receiveReliableIndexes.end()) {
                if (*it == receiveReliableIndexMin + 1) {
                    receiveReliableIndexes.erase(it++);
                    receiveReliableIndexMin++;
                } else {
                    it++;
                }
            }
            return true;
        }
        receiveReliableIndexes.insert(index);
        return true;
    }
    bool handleSequencedIndex(int index, int channel) {
        if (index > receiveSequencedIndex[channel]) {
            receiveSequencedIndex[channel] = index;
            return true;
        }
        return false;
    }
    void readAndHandlePacket(std::vector<char> data);
    void handleFragmentedPacket(std::vector<char> data, int compoundSize, int compoundId, int index, int orderIndex,
                                byte orderChannel);
    void handleOrderedPacket(std::vector<char> data, int orderIndex, byte orderChannel);
    inline bool isPacketNextInOrderedQueue(int orderIndex, byte orderChannel) {
        return (receiveOrderIndex[orderChannel] == orderIndex);
    }
    void incrementOrderIndex(byte orderChannel);

    void resendPackets();

    void sendPing();

    /**
     * This function returns the time that took to to receive a pong after sending a ping, in ms.
     */
    inline int getPing() { return pingTime; }

    inline bool isMostLikelyTimeouted() {
        std::lock_guard<std::mutex> lock (packetTimesMutex);
        return (timeoutPacketAttempts > 0);
    }

    void updateLastPacketReceiveTime();

};


