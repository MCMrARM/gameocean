#include "Protocol.h"

std::map<std::string, Protocol*> Protocol::protocols = std::map<std::string, Protocol*>();

template<typename T>
void Protocol::registerPacket() {
    if (T::TYPE == Packet::Type::BOTH || T::TYPE == Packet::Type::CLIENTBOUND) {
        clientPackets[T::ID] = packet<T>;
    }
    if (T::TYPE == Packet::Type::BOTH || T::TYPE == Packet::Type::SERVERBOUND) {
        serverPackets[T::ID] = packet<T>;
    }
}

Packet *Protocol::getPacket(int id, bool client) {
    if (client) {
        if (clientPackets.count(id) <= 0) { return nullptr; }
        CreatePacket *pk = clientPackets[id];
        return pk();
    } else {
        if (serverPackets.count(id) <= 0) { return nullptr; }
        CreatePacket *pk = serverPackets[id];
        return pk();
    }
}
