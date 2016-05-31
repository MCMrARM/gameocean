#include <gameocean/utils/Random.h>
#include <climits>
#include "RakNetProtocolServer.h"
#include "RakNetPacketIds.h"
#include "RakNetProtocol.h"
#include "packet/RakNetConnectReplyPacket.h"
#include "packet/RakNetACKPacket.h"

RakNetProtocolServer::RakNetProtocolServer(Protocol &protocol) : ProtocolServer(protocol) {
    serverId = Random::instance.nextInt(0, INT_MAX);
}

void RakNetProtocolServer::loop() {
    if (!socket.listen("", (unsigned short) port, Socket::Protocol::UDP)) {
        Logger::main->error("RakNetProtocolServer", "Failed to start listening");
        return;
    }
    Datagram dg;
    while (true) {
        if (shouldStop)
            break;
        dg = socket.receiveDatagram();
        if (!dg.valid)
            break;
        MemoryBinaryStream stream((byte *) dg.data, (int) dg.dataSize);
        stream.swapEndian = true;
        byte pkId;
        stream >> pkId;
        Logger::main->trace("RakNetProtocolServer", "Received packet id %i (size: %i)", pkId, stream.getRemainingSize());
        if (pkId == RAKNET_PACKET_UNCONNECTED_PING) { // ping: int64 id, MAGIC magic
            if (stream.getRemainingSize() < 8)
                continue;
            long long pingId;
            stream >> pingId;
            if (!RakNetProtocol::checkRakNetMagicBytes(stream))
                continue;
            std::lock_guard<std::mutex> lock(serverNameMutex);
            Datagram replyDatagram;
            replyDatagram.addr = dg.addr;
            MemoryBinaryStream resp((byte *) replyDatagram.data, sizeof(replyDatagram.data));
            resp.swapEndian = true;
            resp << (byte) RAKNET_PACKET_UNCONNECTED_PONG;
            resp << pingId << serverId;
            resp.write(RakNetPacketMagic, sizeof(RakNetPacketMagic));
            resp << (unsigned short) serverName.size();
            resp.write((byte *) serverName.c_str(), (unsigned short) serverName.size());
            replyDatagram.dataSize = resp.getSize();
            socket.sendDatagram(replyDatagram);
        } else if (pkId == RAKNET_PACKET_CONNECT_MTU_REQUEST) {
            if (!RakNetProtocol::checkRakNetMagicBytes(stream))
                continue;
            byte protocolVersion;
            stream >> protocolVersion;
            if (protocolVersion != 7) {
                Logger::main->trace("RakNetProtocolServer", "Invalid protocol version: %i", protocolVersion);
                continue;
            }

            Datagram replyDatagram;
            replyDatagram.addr = dg.addr;
            MemoryBinaryStream resp((byte *) replyDatagram.data, sizeof(replyDatagram.data));
            resp.swapEndian = true;
            resp << (byte) RAKNET_PACKET_CONNECT_MTU_REPLY;
            resp.write(RakNetPacketMagic, sizeof(RakNetPacketMagic));
            resp << serverId;
            resp << (byte) 0; // server security
            resp << (unsigned short) dg.dataSize;
            replyDatagram.dataSize = resp.getSize();
            socket.sendDatagram(replyDatagram);
        } else if (pkId == RAKNET_PACKET_CONNECT_REQUEST) {
            if (!RakNetProtocol::checkRakNetMagicBytes(stream))
                continue;
            byte security; int securityCookie;
            short serverPort, mtuSize;
            long long clientId;
            stream >> security >> securityCookie >> serverPort >> mtuSize >> clientId;

            if (serverPort != port)
                continue;

            Logger::main->trace("RakNetProtocolServer", "An client has connected");

            std::shared_ptr<RakNetConnection> connection (createRakNetConnection(dg.addr));
            connection->setPrefferedMTU(mtuSize);
            clients[(sockaddr&) dg.addr] = connection;
            RakNetConnectReplyPacket pk;
            pk.serverId = serverId;
            pk.clientAddr = (sockaddr const&) dg.addr;
            pk.mtu = (short) connection->getMTU();
            pk.clientId = clientId;
            connection->sendRaw(pk);
        } else if (pkId >= 0x80 && pkId < 0x8e) {
            if (clients.count((sockaddr&) dg.addr) <= 0)
                continue;
            std::shared_ptr<RakNetConnection> connection = clients.at((sockaddr&) dg.addr);
            int index = 0;
            stream.read((byte*) &index, 3);
            {
                RakNetACKPacket ackPacket;
                ackPacket.addPacketId(index);
                connection->sendRaw(ackPacket);
            }
            //Logger::main->trace("RakNetProtocolServer", "Frame set index: %i", index);
            while (stream.getRemainingSize() > 0) {
                char flags;
                unsigned short length;
                stream >> flags >> length;
                unsigned short orgLength = length;
                length = (unsigned short) ((length + 7) / 8);
                RakNetReliability type = (RakNetReliability) (flags >> 5);
                bool isFragmented = (flags & 0b10000) != 0;
                if (RakNetIsTypeReliable(type)) {
                    int relIndex = 0;
                    stream.read((byte*) &relIndex, 3);
                }
                if (RakNetIsTypeSequenced(type)) {
                    int seqIndex = 0;
                    stream.read((byte*) &seqIndex, 3);
                }
                if (RakNetIsTypeOrdered(type)) {
                    int frameIndex = 0;
                    stream.read((byte*) &frameIndex, 3);
                    byte channel;
                    stream >> channel;
                }
                if (isFragmented) {
                    int csize;
                    short cid;
                    int cindex;
                    stream >> csize >> cid >> cindex;
                    std::vector<char> data;
                    data.resize(length);
                    stream.read((byte *) data.data(), length);
                    connection->handleFragmentedPacket(std::move(data), csize, cid, cindex);
                } else {
                    unsigned int startOff = stream.getPos();
                    Packet *pk = protocol.readPacket(stream, false);
                    unsigned int readSize = stream.getPos() - startOff;
                    if (readSize > length) {
                        Logger::main->error("RakNetProtocolServer", "Read too much data (read %i but was supposed to read only %i)", readSize, length);
                    } else if (readSize < length) {
                        Logger::main->trace("RakNetProtocolServer", "Read not enough data (read %i but was supposed to read %i)", readSize, length);
                        char filler[length - readSize];
                        stream.read((byte *) filler, sizeof(filler));
                    }
                    if (pk != nullptr) {
                        Logger::main->trace("RakNetProtocolServer", "Received sub-packet; id: %i, length: %i", pk->getId(), length);
                        connection->handlePacket(pk);
                        delete pk;
                    }
                }
            }
        } else if (pkId == RAKNET_PACKET_ACK) {
            // it, sadly, has a special case
            RakNetACKPacket pk;
            pk.read(stream);
            pk.handleServer(*clients.at((sockaddr&) dg.addr));
        } else {
            Logger::main->warn("RakNetProtocolServer", "Unknown packet: %i", pkId);
            continue;
        }
    }
    socket.close();
    Logger::main->trace("RakNetProtocolServer", "Finished listening");
}

void RakNetProtocolServer::removeConnection(RakNetConnection &connection) {
    clients.erase((const sockaddr &) connection.getSockAddr());
}