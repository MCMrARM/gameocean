#include "Packet.h"

#include "Socket.h"
#include "Connection.h"
#include "server/ConnectPacket.h"
#include "client/ConnectedPacket.h"
#include "client/DisconnectedPacket.h"

std::map<int, Packet::CreatePacket *> Packet::clientPackets;
std::map<int, Packet::CreatePacket *> Packet::serverPackets;

template<typename T>
void Packet::registerPacket(int id, Packet::Type type) {
    if (type == Packet::Type::BOTH || type == Packet::Type::CLIENTBOUND) {
        Packet::clientPackets[id] = Packet::packet<T>;
    }
    if (type == Packet::Type::BOTH || type == Packet::Type::SERVERBOUND) {
        Packet::serverPackets[id] = Packet::packet<T>;
    }
}

void Packet::registerCommonPackets() {
    Packet::registerPacket<ConnectPacket>(ConnectPacket::ID, ConnectPacket::TYPE);
    Packet::registerPacket<ConnectedPacket>(ConnectedPacket::ID, ConnectedPacket::TYPE);
    Packet::registerPacket<DisconnectedPacket>(DisconnectedPacket::ID, DisconnectedPacket::TYPE);
}

Packet *Packet::getPacket(int id, bool client) {
    if (client) {
        if (Packet::clientPackets.count(id) <= 0) { return null; }
        Packet::CreatePacket *pk = Packet::clientPackets[id];
        return pk();
    } else {
        if (Packet::serverPackets.count(id) <= 0) { return null; }
        Packet::CreatePacket *pk = Packet::serverPackets[id];
        return pk();
    }
}

Packet *Packet::getPacket(BinaryStream &stream, bool client) {
    int pkLen;
    stream >> pkLen;
    Logger::main->trace("Packet", "Received packet with size: %i", pkLen);

    byte packet[pkLen];
    stream.read(packet, pkLen);

    MemoryBinaryStream pkStream(packet, pkLen);
    unsigned char pkId;
    pkStream >> pkId;
    Logger::main->trace("Packet", "Received packet with id: %i", pkId);

    Packet *pk = Packet::getPacket(pkId, client);
    if (pk == null) {
        Logger::main->warn("Packet", "Unknown packet received: %i, size: %i", pkId, pkLen);
        pk = new UnknownPacket(pkId);
    }
    pk->read(pkStream);
    return pk;
}

void Packet::sendPacket(BinaryStream &stream, const Packet &packet) {
    int pkLen = BinaryStream::BYTE_SIZE + packet.getPacketSize();
    int bufLen = BinaryStream::INT_SIZE + pkLen;

    byte buf[bufLen];
    MemoryBinaryStream pkStream(buf, bufLen);

    char pkId = (char) packet.getId();
    pkStream << pkLen << pkId;
    packet.write(pkStream);

    stream.write(buf, bufLen);
}

void Packet::sendPacket(Connection &connection, const Packet &packet) {
    Packet::sendPacket(connection.socket->getStream(), packet);
}