#include "RakNetConnection.h"
#include <gameocean/net/Socket.h>
#include "RakNetProtocolServer.h"

RakNetConnection::RakNetConnection(RakNetProtocolServer &server, sockaddr_in addr) : Connection(server.getProtocol(), false),
                                                                                     server(server), addr(addr) {

}

void RakNetConnection::setPrefferedMTU(int mtu) {
    this->mtu = std::min(mtu, 1492);
}

void RakNetConnection::close() {
    server.removeConnection(*this);
}

int RakNetConnection::send(Packet &packet, RakNetReliability reliability) {
    // base packet overhead: 1 (pk id) + 3 (frame count) + 1 (flags) + 2 (length)
    // additional: reliable - 3, sequenced - 3, ordered - 4
    bool needsFragmenting = (packet.getPacketSize() + 1 >= mtu - 7 - (RakNetIsTypeReliable(reliability) ? 3 : 0)
                                                      - (RakNetIsTypeSequenced(reliability) ? 3 : 0)
                                                      - (RakNetIsTypeOrdered(reliability) ? 4 : 0));
    if (needsFragmenting)
        abort();
    Datagram dg;
    dg.addr = addr;
    MemoryBinaryStream stream((byte *) dg.data, sizeof(dg.data));
    stream.swapEndian = true;
    stream << (char) 0x84;
    stream.write((byte *) &sendFrameIndex, 3);
    sendFrameIndex++;
    int flags = ((int) reliability) << 5;
    stream << (char) flags << (short) ((packet.getPacketSize() + 1) * 8);
    if (RakNetIsTypeReliable(reliability)) {
        abort();
    }
    if (RakNetIsTypeSequenced(reliability)) {
        int seqId = sendSequencedIndex++;
        stream.write((byte*) &seqId, 3);
    }
    if (RakNetIsTypeOrdered(reliability)) {
        abort();
    }
    stream << (byte) packet.getId();
    packet.write(stream);
    dg.dataSize = stream.getSize();
    server.getSocket().sendDatagram(dg);
    return 0;
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