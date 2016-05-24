#include <gameocean/utils/Random.h>
#include <climits>
#include "RakNetProtocolServer.h"
#include "RakNetPacketIds.h"
#include "RakNetProtocol.h"

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
        } else {
            Logger::main->warn("RakNetProtocolServer", "Unknown packet: %i", pkId);
            break;
        }
    }
    socket.close();
    Logger::main->trace("RakNetProtocolServer", "Finished listening");
}