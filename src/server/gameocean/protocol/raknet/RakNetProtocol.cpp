#include "RakNetProtocol.h"
#include "RakNetPacketIds.h"
#include <unistd.h>
#include "packet/RakNetConnectReplyPacket.h"
#include "packet/RakNetOnlineConnectRequestPacket.h"
#include "packet/RakNetOnlineConnectReplyPacket.h"
#include "packet/RakNetClientHandshakePacket.h"
#include "packet/RakNetACKPacket.h"
#include "packet/RakNetNAKPacket.h"
#include "packet/RakNetPingPacket.h"
#include "packet/RakNetPongPacket.h"

RakNetProtocol::RakNetProtocol() {
    registerPacket<RakNetConnectReplyPacket>();
    registerPacket<RakNetOnlineConnectRequestPacket>();
    registerPacket<RakNetOnlineConnectReplyPacket>();
    registerPacket<RakNetClientHandshakePacket>();
    registerPacket<RakNetACKPacket>();
    registerPacket<RakNetNAKPacket>();
    registerPacket<RakNetPingPacket>();
    registerPacket<RakNetPongPacket>();
}

Packet *RakNetProtocol::readPacket(BinaryStream &stream, bool client) {
    unsigned char pkId;
    stream >> pkId;

    Packet* pk = getPacket(pkId, client);
    if (pk == nullptr) {
        Logger::main->warn("RakNetProtocol", "Unknown packet received: %i", pkId);
        pk = new UnknownPacket(pkId);
    }
    pk->read(stream);
    return pk;
}
void RakNetProtocol::writePacket(BinaryStream &stream, Packet &packet) {
    char pkId = (char) packet.getId();
    stream << pkId;
    packet.write(stream);
}

bool RakNetProtocol::checkRakNetMagicBytes(char magic[16]) {
    return (memcmp(magic, RakNetPacketMagic, 16) == 0);
}
bool RakNetProtocol::checkRakNetMagicBytes(BinaryStream &stream) {
    char magic[16];
    stream.readFully((byte*) magic, 16);
    return checkRakNetMagicBytes(magic);
}
sockaddr RakNetProtocol::readRakNetAddress(BinaryStream &stream) {
    unsigned char ver;
    stream >> ver;
    sockaddr ret;
    memset(&ret, 0, sizeof(ret));
    if (ver == 4) {
        sockaddr_in i;
        stream >> i.sin_addr.s_addr >> i.sin_port;
        i.sin_port = ntohs(i.sin_port);
        memcpy(&ret, &i, sizeof(sockaddr));
    }
    return ret;
}
void RakNetProtocol::writeRakNetAddress(BinaryStream &stream, sockaddr const &addr) {
    if (addr.sa_family == AF_INET) {
        stream << (char) 4;
        sockaddr_in const &addr4 = (sockaddr_in const &) addr;
        stream << addr4.sin_addr.s_addr << ntohs(addr4.sin_port);
    }
}
long long RakNetProtocol::getTimeForPing() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}