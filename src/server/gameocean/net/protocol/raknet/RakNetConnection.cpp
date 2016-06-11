#include "RakNetConnection.h"
#include <gameocean/net/Socket.h>
#include "RakNetProtocolServer.h"
#include "packet/RakNetPingPacket.h"
#include "packet/RakNetDetectLostConnectionsPacket.h"

RakNetConnection::RakNetConnection(RakNetProtocolServer &server, sockaddr_in addr) : Connection(server.getProtocol(), false),
                                                                                     server(server), addr(addr) {
    memset(sendSequencedIndex, 0, sizeof(sendSequencedIndex));
    memset(sendOrderIndex, 0, sizeof(sendOrderIndex));
    memset(receiveOrderIndex, 0, sizeof(receiveOrderIndex));
    for (int i = 0; i < 256; i++)
        receiveSequencedIndex[i] = -1;
    updateLastPacketReceiveTime();
    lastTimeoutCheckPacketSentTime = std::chrono::steady_clock::now();
}

void RakNetConnection::setPrefferedMTU(int mtu) {
    this->mtu = (unsigned int) std::min(mtu, 1492);
}

void RakNetConnection::doClose() {
    server.removeConnection(*this);
}

int RakNetConnection::sendFrame(SendFrame &frame) {
    int ret = 0;
    bool isResending = false;
    Datagram dg;
    dg.addr = addr;
    MemoryBinaryStream stream((byte *) dg.data, sizeof(dg.data));
    stream.swapEndian = true;
    stream << (char) 0x84;
    int myIndex = sendFrameIndex++;
    stream.write((byte *) &myIndex, 3);
    RakNetReliability sendReliability = frame.reliability;
    bool needsACKReceipt = RakNetTypeNeedsACKReceipt(sendReliability);
    if (sendReliability == RakNetReliability::RELIABLE_ACK_RECEIPT)
        sendReliability = RakNetReliability::RELIABLE;
    else if (sendReliability == RakNetReliability::RELIABLE_ORDERED_ACK_RECEIPT)
        sendReliability = RakNetReliability::RELIABLE_ORDERED;
    int flags = ((int) sendReliability) << 5;
    if (frame.compound != nullptr)
        flags |= 0b10000;
    stream << (char) flags << (short) (frame.data.size() * 8);
    if (RakNetIsTypeReliable(frame.reliability)) {
        if (frame.reliableIndex == -1)
            frame.reliableIndex = sendReliableIndex++;
        else
            isResending = true;
        stream.write((byte*) &frame.reliableIndex, 3);
    }
    if (RakNetIsTypeSequenced(frame.reliability)) {
        if (frame.compound == nullptr) {
            if (frame.seqencedIndex == -1)
                frame.seqencedIndex = sendSequencedIndex[frame.orderChannel]++;
            stream.write((byte *) &frame.seqencedIndex, 3);
        } else {
            if (frame.compound->sequencedIndex == -1)
                frame.compound->sequencedIndex = sendSequencedIndex[frame.compound->orderChannel]++;
            stream.write((byte *) &frame.compound->sequencedIndex, 3);
        }
    }
    if (RakNetIsTypeOrdered(frame.reliability)) {
        if (frame.compound == nullptr) {
            if (frame.orderIndex == -1)
                frame.orderIndex = sendOrderIndex[frame.orderChannel]++;
            stream.write((byte*) &frame.orderIndex, 3);
            stream << (byte) frame.orderChannel;
        } else {
            if (frame.compound->orderIndex == -1)
                frame.compound->orderIndex = sendOrderIndex[frame.compound->orderChannel]++;
            stream.write((byte*) &frame.compound->orderIndex, 3);
            stream << (byte) frame.compound->orderChannel;
        }
    }
    if (frame.compound != nullptr) {
        stream << frame.compound->frameCount << (short) frame.compound->id << frame.compoundIndex;
    }
    if (frame.compound == nullptr && needsACKReceipt) {
        ret = ackReceiptId++;
        if (RakNetIsTypeReliable(frame.reliability)) {
            frame.reliableACKReceiptId = ret;
        } else {
            sentPackets[myIndex].unreliableAckReceiptId = ret;
        }
    }
    stream.write((byte*) &frame.data[0], (unsigned int) frame.data.size());
    if (RakNetIsTypeReliable(frame.reliability)) {
        if (!isResending) {
            sendReliableFrames[frame.reliableIndex] = frame;
        }
        sentPackets[myIndex].reliableFrameId = frame.reliableIndex;
        queueReliablePacketResend(frame.reliableIndex, std::chrono::milliseconds(5000));
    }
    dg.dataSize = stream.getSize();
    server.getSocket().sendDatagram(dg);
    return ret;
}

void RakNetConnection::queueReliablePacketResend(int frameId, std::chrono::milliseconds resendTime) {
    std::lock_guard<std::recursive_mutex> lock (sendReliableMutex);
    std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now() + resendTime;
    if (sendReliableQueue.count(frameId) <= 0 || sendReliableQueue.at(frameId) > tp)
        sendReliableQueue[frameId] = tp;
}

void RakNetConnection::dequeueReliablePacketResend(int frameId) {
    std::lock_guard<std::recursive_mutex> lock (sendReliableMutex);
    sendReliableQueue.erase(frameId);
}

void RakNetConnection::onPacketDelivered(int pkId) {
    if (sentPackets.count(pkId)) {
        RakNetConnection::PacketMeta &meta = sentPackets.at(pkId);
        if (meta.unreliableAckReceiptId != -1 && getRakNetHandler() != nullptr)
            getRakNetHandler()->onPacketDelivered(*this, meta.unreliableAckReceiptId);
        if (meta.reliableFrameId != -1)
            onReliableFrameDelivered(meta.reliableFrameId);
        sentPackets.erase(pkId);
    }
}

void RakNetConnection::onPacketLost(int pkId) {
    if (sentPackets.count(pkId)) {
        RakNetConnection::PacketMeta &meta = sentPackets.at(pkId);
        if (meta.unreliableAckReceiptId != -1 && getRakNetHandler() != nullptr)
            getRakNetHandler()->onPacketLost(*this, meta.unreliableAckReceiptId);
        if (meta.reliableFrameId != -1)
            queueReliablePacketResend(meta.reliableFrameId, std::chrono::milliseconds(10));
        sentPackets.erase(pkId);
    }
}

void RakNetConnection::onReliableFrameDelivered(int frameId) {
    std::lock_guard<std::recursive_mutex> lock (sendReliableMutex);
    if (sendReliableFrames.count(frameId) <= 0)
        return;
    dequeueReliablePacketResend(frameId);
    SendFrame &frame = sendReliableFrames.at(frameId);
    if (frame.compound != nullptr) {
        frame.compound->frameRefs--;
        if (frame.compound->frameRefs <= 0) {
            if (frame.compound->ackReceiptId != -1 && getRakNetHandler() != nullptr)
                getRakNetHandler()->onPacketDelivered(*this, frame.compound->ackReceiptId);
            delete frame.compound;
        }
    }
    if (frame.reliableACKReceiptId != -1 && getRakNetHandler() != nullptr)
        getRakNetHandler()->onPacketDelivered(*this, frame.reliableACKReceiptId);
    sendReliableFrames.erase(frameId);
}

int RakNetConnection::send(Packet &packet, RakNetReliability reliability, int orderChannel) {
    unsigned int packetSize = packet.getPacketSize();
    // base packet overhead: 1 (pk id) + 3 (frame count) + 1 (flags) + 2 (length)
    // additional: reliable - 3, sequenced - 3, ordered - 4
    bool needsFragmenting = (packetSize + 1 >= mtu - 7 - (RakNetIsTypeReliable(reliability) ? 3 : 0)
                                               - (RakNetIsTypeSequenced(reliability) ? 3 : 0)
                                               - (RakNetIsTypeOrdered(reliability) ? 4 : 0));
    std::vector<char> data;
    data.resize(packetSize + 1);
    MemoryBinaryStream stream ((byte *) &data[0], packetSize + 1);
    stream.swapEndian = true;
    stream << (byte) packet.getId();
    packet.write(stream);
    if (needsFragmenting) {
        // make it reliable
        if (reliability == RakNetReliability::UNRELIABLE)
            reliability = RakNetReliability::RELIABLE;
        else if (reliability == RakNetReliability::UNRELIABLE_SEQUENCED)
            reliability = RakNetReliability::RELIABLE_SEQUENCED;
        else if (reliability == RakNetReliability::UNRELIABLE_ACK_RECEIPT)
            reliability = RakNetReliability::RELIABLE_ACK_RECEIPT;

        // fragment it!
        unsigned int off = 0;
        unsigned int singleSize = mtu - 7 - 3 - (RakNetIsTypeSequenced(reliability) ? 3 : 0)
                                  - (RakNetIsTypeOrdered(reliability) ? 4 : 0)
                                  - 4 - 2 - 4;
        SendFrameCompound *compound = new SendFrameCompound();
        compound->id = sendCompoundIndex++;
        compound->frameCount = (packetSize + 1 + singleSize - 1) / singleSize;
        compound->frameRefs = compound->frameCount;
        compound->orderChannel = (char) orderChannel;
        if (RakNetTypeNeedsACKReceipt(reliability))
            compound->ackReceiptId = ackReceiptId++;
        int compoundIndex = 0;
        while (off < packetSize + 1) {
            SendFrame frame;
            unsigned int frameSize = std::min(singleSize, (unsigned int) data.size() - off);
            frame.data = std::vector<char>(frameSize);
            memcpy(frame.data.data(), &data[off], frameSize);
            frame.reliability = reliability;
            frame.compound = compound;
            frame.compoundIndex = compoundIndex++;
            sendFrame(frame);
            off += frameSize;
        }
        return (compound->ackReceiptId == -1 ? 0 : compound->ackReceiptId);
    } else {
        SendFrame frame;
        frame.data = std::move(data);
        frame.reliability = reliability;
        frame.orderChannel = (char) orderChannel;
        return sendFrame(frame);
    }
}

void RakNetConnection::sendRaw(Packet &packet) {
    Datagram dg;
    dg.addr = addr;
    if (packet.getPacketSize() + 1 > mtu) {
        Logger::main->warn("RakNetConnection", "Cannot send packet - packet size %i is bigger than MTU %i",
                           packet.getPacketSize(), mtu);
        return;
    }
    MemoryBinaryStream stream((byte *) dg.data, sizeof(dg.data));
    stream.swapEndian = true;
    stream << (byte) packet.getId();
    packet.write(stream);
    dg.dataSize = stream.getSize();
    server.getSocket().sendDatagram(dg);
}

void RakNetConnection::readAndHandlePacket(std::vector<char> data) {
    MemoryBinaryStream stream((byte *) data.data(), (unsigned int) data.size());
    stream.swapEndian = true;
    Packet *pk = protocol.readPacket(stream, false);
    if (pk != nullptr) {
        Logger::main->trace("RakNetConnection", "Received packet; id: %i, length: %i", pk->getId(), data.size());
        handlePacket(pk);
        delete pk;
    } else {
        Logger::main->trace("RakNetConnection", "Received unknown packet; id: %i, length: %i", data[0], data.size());
    }
}

void RakNetConnection::handleFragmentedPacket(std::vector<char> data, int compoundSize, int compoundId, int index,
                                              int orderIndex, byte orderChannel) {
    if (receiveCompounds.count(compoundId) <= 0)
        receiveCompounds[compoundId] = RakNetCompound((unsigned int) compoundSize);
    RakNetCompound &compound = receiveCompounds.at(compoundId);
    compound.setOrderInfo(orderIndex, orderChannel);
    compound.addFrame((unsigned int) index, std::move(data));
    if (compound.isComplete()) {
        std::vector<char> built = std::move(compound.build());
        if (!compound.isOrdered()) {
            readAndHandlePacket(std::move(built));
        } else {
            handleOrderedPacket(std::move(built), orderIndex, orderChannel);
        }
        receiveCompounds.erase(compoundId);
    }
}

void RakNetConnection::handleOrderedPacket(std::vector<char> data, int orderIndex, byte orderChannel) {
    if (isPacketNextInOrderedQueue(orderIndex, orderChannel)) {
        readAndHandlePacket(std::move(data));
        incrementOrderIndex(orderChannel);
        return;
    }
    receiveOrderedQueue[{orderChannel, orderIndex}] = std::move(data);
}

void RakNetConnection::incrementOrderIndex(byte orderChannel) {
    auto it = receiveOrderedQueue.find({orderChannel, ++receiveOrderIndex[orderChannel]});
    while (it != receiveOrderedQueue.end()) {
        if (it->first.first == orderChannel && it->first.second == receiveOrderIndex[orderChannel]) {
            readAndHandlePacket(std::move(it->second));
            it = receiveOrderedQueue.erase(it);
            receiveOrderIndex[orderChannel]++;
        }
    }
}

void RakNetConnection::resendPackets() {
    std::vector<int> requeued;
    std::lock_guard<std::recursive_mutex> lock(sendReliableMutex);
    std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();
    for (auto it = sendReliableQueue.begin(); it != sendReliableQueue.end();) {
        if (tp >= it->second) {
            requeued.push_back(it->first);
            it = sendReliableQueue.erase(it);
        } else
            it++;
    }
    for (int pkId : requeued)
        sendFrame(sendReliableFrames[pkId]);

    // also handle ping timeouts
    packetTimesMutex.lock();
    if (tp - lastPacketTime > std::chrono::milliseconds(server.timeoutTime)) {
        bool needsResend = (tp - lastTimeoutCheckPacketSentTime) > std::chrono::milliseconds(server.timeoutResendDelay);
        if (needsResend) {
            timeoutPacketAttempts++;
            if (timeoutPacketAttempts >= server.timeoutAttemptCount) {
                packetTimesMutex.unlock();
                close(DisconnectReason::TIMEOUT, "");
                return;
            }
        }
        packetTimesMutex.unlock();
        if (needsResend) {
            RakNetDetectLostConnectionsPacket pk;
            send(pk, RakNetReliability::UNRELIABLE);
            std::lock_guard<std::mutex> guard(packetTimesMutex);
            lastTimeoutCheckPacketSentTime = tp;
        }
    } else {
        packetTimesMutex.unlock();
    }
}

void RakNetConnection::sendPing() {
    RakNetPingPacket ping;
    ping.time = RakNetProtocol::getTimeForPing();
    send(ping, RakNetReliability::UNRELIABLE);
}

void RakNetConnection::updateLastPacketReceiveTime() {
    std::lock_guard<std::mutex> guard(packetTimesMutex);
    lastPacketTime = std::chrono::steady_clock::now();
    timeoutPacketAttempts = 0;
}