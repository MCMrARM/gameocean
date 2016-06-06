#include "RakNetOnlineConnectRequestPacket.h"
#include "RakNetOnlineConnectReplyPacket.h"
#include "RakNetClientHandshakePacket.h"
#include "RakNetDisconnectPacket.h"
#include "RakNetPingPacket.h"
#include "RakNetPongPacket.h"
#include "RakNetACKPacket.h"
#include "RakNetNAKPacket.h"

void RakNetOnlineConnectRequestPacket::handleServer(Connection &connection) {
    RakNetOnlineConnectReplyPacket reply;
    reply.clientAddr = (const sockaddr &) ((RakNetConnection &) connection).getSockAddr();
    reply.requestTime = time;
    reply.time = RakNetProtocol::getTimeForPing();
    ((RakNetConnection &) connection).send(reply, RakNetReliability::UNRELIABLE);
}

void RakNetClientHandshakePacket::handleServer(Connection &connection) {
    ((RakNetConnection &) connection).pingTime = (int) (RakNetProtocol::getTimeForPing() - requestTime);
    ((RakNetConnection &) connection).setAccepted(true);
}

void RakNetDisconnectPacket::handleServer(Connection &connection) {
    Logger::main->trace("RakNet/Disconnect", "A client has disconnected");
    connection.close(Connection::DisconnectReason::CLOSED);
}

void RakNetPingPacket::handleServer(Connection &connection) {
    RakNetPongPacket pong;
    pong.pingTime = time;
    pong.time = RakNetProtocol::getTimeForPing();
    ((RakNetConnection &) connection).send(pong, RakNetReliability::UNRELIABLE);
}

void RakNetPongPacket::handleServer(Connection &connection) {
    ((RakNetConnection &) connection).pingTime = (int) (RakNetProtocol::getTimeForPing() - pingTime);
}

void RakNetACKPacket::handleServer(Connection &connection) {
    RakNetConnection &rakNetConnection = (RakNetConnection &) connection;
    if (rakNetConnection.sentPackets.size() <= 0)
        return;
    for (Range &range : ranges) {
        range.min = std::max(range.min, (unsigned int) rakNetConnection.sentPackets.begin()->first);
        range.max = std::min(range.max, (unsigned int) (--rakNetConnection.sentPackets.end())->first);
        for (int pkId = range.min; pkId <= range.max; pkId++) {
            rakNetConnection.onPacketDelivered(pkId);
        }
    }
}

void RakNetNAKPacket::handleServer(Connection &connection) {
    RakNetConnection &rakNetConnection = (RakNetConnection &) connection;
    if (rakNetConnection.sentPackets.size() <= 0)
        return;
    for (Range &range : ranges) {
        range.min = std::max(range.min, (unsigned int) rakNetConnection.sentPackets.begin()->first);
        range.max = std::min(range.max, (unsigned int) (--rakNetConnection.sentPackets.end())->first);
        for (int pkId = range.min; pkId <= range.max; pkId++) {
            rakNetConnection.onPacketLost(pkId);
        }
    }
}