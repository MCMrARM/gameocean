#include "RakNetOnlineConnectRequestPacket.h"
#include "RakNetOnlineConnectReplyPacket.h"
#include "RakNetClientHandshakePacket.h"
#include "RakNetPingPacket.h"
#include "RakNetPongPacket.h"

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