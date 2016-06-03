#include "RakNetConnection.h"
#include <gameocean/net/Socket.h>
#include "RakNetProtocolServer.h"

RakNetConnection::RakNetConnection(RakNetProtocolServer &server, sockaddr_in addr) : Connection(server.getProtocol(), false),
                                                                                     server(server), addr(addr) {
    memset(sendSequencedIndex, 0, sizeof(sendSequencedIndex));
    memset(sendOrderIndex, 0, sizeof(sendOrderIndex));
    for (int i = 0; i < 256; i++)
        receiveSequencedIndex[i] = -1;
}

void RakNetConnection::setPrefferedMTU(int mtu) {
    this->mtu = (unsigned int) std::min(mtu, 1492);
}

void RakNetConnection::close() {
    server.removeConnection(*this);
}

int RakNetConnection::sendFrame(SendFrame &frame) {
    int ret = 0;
    Datagram dg;
    dg.addr = addr;
    MemoryBinaryStream stream((byte *) dg.data, sizeof(dg.data));
    stream.swapEndian = true;
    stream << (char) 0x84;
    int myIndex = sendFrameIndex++;
    stream.write((byte *) &myIndex, 3);
    int flags = ((int) frame.reliability) << 5;
    if (frame.compound != nullptr)
        flags |= 0b10000;
    stream << (char) flags << (short) (frame.data.size() * 8);
    if (RakNetIsTypeReliable(frame.reliability)) {
        if (frame.reliableIndex == -1)
            frame.reliableIndex = sendReliableIndex++;
        stream.write((byte*) &frame.reliableIndex, 3);
        //TODO:
    }
    if (RakNetIsTypeSequenced(frame.reliability)) {
        if (frame.seqencedIndex == -1)
            frame.seqencedIndex = sendSequencedIndex[frame.orderChannel]++;
        stream.write((byte*) &frame.seqencedIndex, 3);
    }
    if (RakNetIsTypeOrdered(frame.reliability)) {
        if (frame.orderIndex == -1)
            frame.orderIndex = sendOrderIndex[frame.orderChannel]++;
        stream.write((byte*) &frame.orderIndex, 3);
        stream << (byte) 0; //channel
    }
    if (frame.compound != nullptr) {
        stream << frame.compound->frameCount << (short) frame.compound->id << frame.compoundIndex;
    }
    if (frame.compound == nullptr && RakNetTypeNeedsACKReceipt(frame.reliability)) {
        ret = ackReceiptId++;
        ackReceiptIds[myIndex] = ret;
    }
    stream.write((byte*) &frame.data[0], (unsigned int) frame.data.size());
    dg.dataSize = stream.getSize();
    server.getSocket().sendDatagram(dg);
    return ret;
}

int RakNetConnection::send(Packet &packet, RakNetReliability reliability) {
    reliability = RakNetReliability::UNRELIABLE;
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
        SendFrameCompound compound;
        compound.id = sendCompoundIndex++;
        compound.frameCount = (packetSize + 1 + singleSize - 1) / singleSize;
        compound.frameRefs = compound.frameCount;
        Logger::main->trace("RakNetConnection", "Sending fragmented frame: id %i, size: %i - %i %i", sendCompoundIndex, compound.frameCount, packetSize, singleSize);
        int compoundIndex = 0;
        while (off < packetSize + 1) {
            SendFrame frame;
            unsigned int frameSize = std::min(singleSize, (unsigned int) data.size() - off);
            frame.data = std::vector<char>(frameSize);
            memcpy(frame.data.data(), &data[off], frameSize);
            frame.reliability = reliability;
            frame.compound = &compound;
            frame.compoundIndex = compoundIndex++;
            sendFrame(frame);
            off += frameSize;
        }
        return 0;
    } else {
        SendFrame frame;
        frame.data = std::move(data);
        frame.reliability = reliability;
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

void RakNetConnection::handleFragmentedPacket(std::vector<char> data, int compoundSize, int compoundId, int index) {
    if (receiveCompounds.count(compoundId) <= 0)
        receiveCompounds[compoundId] = RakNetCompound((unsigned int) compoundSize);
    RakNetCompound &compound = receiveCompounds.at(compoundId);
    compound.addFrame((unsigned int) index, std::move(data));
    if (compound.isComplete()) {
        std::vector<char> built = std::move(compound.build());
        MemoryBinaryStream stream ((byte *) built.data(), (unsigned int) built.size());
        Packet *pk = protocol.readPacket(stream, false);
        if (pk != nullptr) {
            Logger::main->trace("RakNet/FragmentedPacket", "Received packet; id: %i, length: %i", pk->getId(), built.size());
            handlePacket(pk);
            delete pk;
        } else {
            Logger::main->trace("RakNet/FragmentedPacket", "Received unknown packet; id: %i, length: %i", built[0], built.size());
        }
        receiveCompounds.erase(compoundId);
    }
}