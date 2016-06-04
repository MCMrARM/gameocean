#include "RakNetOnlineConnectRequestPacket.h"
#include "RakNetOnlineConnectReplyPacket.h"
#include "RakNetClientHandshakePacket.h"
#include "RakNetPingPacket.h"
#include "RakNetPongPacket.h"
#include "RakNetACKPacket.h"

void RakNetOnlineConnectRequestPacket::handleServer(Connection &connection) {
    RakNetOnlineConnectReplyPacket reply;
    reply.clientAddr = (const sockaddr &) ((RakNetConnection &) connection).getSockAddr();
    reply.requestTime = time;
    reply.time = RakNetProtocol::getTimeForPing();
    ((RakNetConnection &) connection).send(reply, RakNetReliability::UNRELIABLE);
}

void RakNetClientHandshakePacket::handleServer(Connection &connection) {
    ((RakNetConnection &) connection).setAccepted(true);
}

void RakNetPingPacket::handleServer(Connection &connection) {
    RakNetPongPacket pong;
    pong.pingTime = time;
    pong.time = RakNetProtocol::getTimeForPing();
    ((RakNetConnection &) connection).send(pong, RakNetReliability::UNRELIABLE);
}

void RakNetPongPacket::handleServer(Connection &connection) {
    //
}

void RakNetACKPacket::handleServer(Connection &connection) {
    RakNetConnection &rakNetConnection = (RakNetConnection &) connection;
    RakNetConnectionHandler *connHandler = rakNetConnection.getRakNetHandler();
    for (Range &range : ranges) {
        for (int pkId = range.min; pkId <= range.max; pkId++) {
            if (rakNetConnection.sentPackets.count(pkId)) {
                RakNetConnection::PacketMeta &meta = rakNetConnection.sentPackets.at(pkId);
                if (meta.unreliableAckReceiptId != -1 && connHandler != nullptr)
                    connHandler->onPacketDelivered(rakNetConnection, meta.unreliableAckReceiptId);
                if (meta.reliableFrameId != -1)
                    rakNetConnection.onReliableFrameReceived(meta.reliableFrameId);
                rakNetConnection.sentPackets.erase(pkId);
            }
        }
    }
}