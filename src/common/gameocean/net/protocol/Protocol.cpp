#include "Protocol.h"

std::map<std::string, Protocol*> Protocol::protocols = std::map<std::string, Protocol*>();

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
