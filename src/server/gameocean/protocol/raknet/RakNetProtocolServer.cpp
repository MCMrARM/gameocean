#include <gameocean/utils/Random.h>
#include <climits>
#include "RakNetProtocolServer.h"
#include "RakNetPacketIds.h"
#include "RakNetProtocol.h"
#include "packet/RakNetConnectReplyPacket.h"
#include "packet/RakNetPongPacket.h"
#include "packet/RakNetACKPacket.h"
#include "packet/RakNetNAKPacket.h"

RakNetProtocolServer::RakNetProtocolServer(Protocol &protocol) : ProtocolServer(protocol), resendThread(*this), pingThread(*this) {
    serverId = Random::instance.nextInt(0, INT_MAX);
}

void RakNetProtocolServer::loop() {
    if (!socket.listen("", (unsigned short) port, Socket::Protocol::UDP)) {
        Logger::main->error("RakNetProtocolServer", "Failed to start listening");
        return;
    }
    resendThread.start();
    pingThread.start();
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
        // Logger::main->trace("RakNetProtocolServer", "Received packet id %i (size: %i)", pkId, stream.getRemainingSize());
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
            connection->setHandler(getHandler());
            if (rakNetHandler != nullptr)
                connection->setRakNetHandler(rakNetHandler);
            connection->setPrefferedMTU(mtuSize);
            clientsMutex.lock();
            clients[(sockaddr&) dg.addr] = connection;
            clientsMutex.unlock();
            RakNetConnectReplyPacket pk;
            pk.serverId = serverId;
            pk.clientAddr = (sockaddr const&) dg.addr;
            pk.mtu = (short) connection->getMTU();
            pk.clientId = clientId;
            connection->sendRaw(pk);
        } else if (pkId >= 0x80 && pkId < 0x8e) {
            clientsMutex.lock();
            if (clients.count((sockaddr&) dg.addr) <= 0) {
                clientsMutex.unlock();
                continue;
            }
            std::shared_ptr<RakNetConnection> connection = clients.at((sockaddr&) dg.addr);
            clientsMutex.unlock();
            int index = 0;
            stream.read((byte*) &index, 3);
            {
                RakNetACKPacket ackPacket;
                ackPacket.addPacketId(index);
                connection->sendRaw(ackPacket);
            }
            while (stream.getRemainingSize() > 3) {
                char flags;
                unsigned short length;
                stream >> flags >> length;
                length = (unsigned short) ((length + 7) / 8);
                RakNetReliability type = (RakNetReliability) (flags >> 5);
                bool isFragmented = (flags & 0b10000) != 0;
                bool ignore = false;
                if (RakNetIsTypeReliable(type)) {
                    int relIndex = 0;
                    stream.read((byte*) &relIndex, 3);
                    if (!connection->handleReliableIndex(relIndex))
                        ignore = true;
                }
                int seqIndex = 0;
                if (RakNetIsTypeSequenced(type)) {
                    stream.read((byte*) &seqIndex, 3);
                }
                int orderIndex = -1;
                byte orderChannel = 0;
                bool isOrdered = false;
                if (RakNetIsTypeOrdered(type)) {
                    orderIndex = 0;
                    stream.read((byte*) &orderIndex, 3);
                    stream >> orderChannel;
                    if (RakNetIsTypeSequenced(type)) {
                        if (!connection->handleSequencedIndex(seqIndex, orderChannel))
                            ignore = true;
                    } else {
                        isOrdered = true;
                    }
                }
                if (ignore) {
                    stream.skip(length);
                    continue;
                }

                if (isFragmented) {
                    int csize;
                    short cid;
                    int cindex;
                    stream >> csize >> cid >> cindex;
                    std::vector<char> data;
                    data.resize(length);
                    stream.read((byte *) data.data(), length);
                    connection->handleFragmentedPacket(std::move(data), csize, cid, cindex, orderIndex, orderChannel);
                } else if (isOrdered && !connection->isPacketNextInOrderedQueue(orderIndex, orderChannel)) {
                    std::vector<char> data;
                    data.resize(length);
                    stream.read((byte *) data.data(), length);
                    connection->handleOrderedPacket(std::move(data), orderIndex, orderChannel);
                } else if (length > 0) {
                    if (isOrdered)
                        connection->incrementOrderIndex(orderChannel);
                    unsigned int startOff = stream.getPos();
                    Packet *pk = protocol.readPacket(stream, false);
                    unsigned int readSize = stream.getPos() - startOff;
                    if (readSize > length) {
                        Logger::main->error("RakNetProtocolServer", "Read too much data (read %i but was supposed to read only %i)", readSize, length);
                        break;
                    } else if (readSize < length) {
                        Logger::main->trace("RakNetProtocolServer", "Read not enough data (read %i but was supposed to read %i)", readSize, length);
                        stream.skip(length - readSize);
                    }
                    if (pk != nullptr) {
                        // Logger::main->trace("RakNetProtocolServer", "Received sub-packet; id: %i, length: %i", pk->getId(), length);
                        connection->handlePacket(pk);
                        delete pk;
                    }
                }
            }
        } else if (pkId == RAKNET_PACKET_ACK) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            RakNetACKPacket pk;
            pk.read(stream);
            pk.handleServer(*clients.at((sockaddr&) dg.addr));
        } else if (pkId == RAKNET_PACKET_NAK) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            RakNetNAKPacket pk;
            pk.read(stream);
            pk.handleServer(*clients.at((sockaddr&) dg.addr));
        } else {
            Logger::main->warn("RakNetProtocolServer", "Unknown packet: %i", pkId);
            continue;
        }
    }
    socket.close();
    Logger::main->trace("RakNetProtocolServer", "Finished listening");
    resendThread.stop();
    pingThread.stop();
}

void RakNetProtocolServer::removeConnection(RakNetConnection &connection) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase((const sockaddr &) connection.getSockAddr());
}