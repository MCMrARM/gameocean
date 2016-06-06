#pragma once

#include <map>
#include "../Packet.h"
#include "../../utils/Thread.h"
#include "../../utils/Logger.h"
#include "../../utils/StringUtils.h"

class ProtocolServer;

class Protocol {

protected:
    typedef Packet *CreatePacket();
    template<typename T> static Packet *packet() { return new T(); };

    std::map<int, CreatePacket *> clientPackets;
    std::map<int, CreatePacket *> serverPackets;

    template<typename T> void registerPacket() {
        const int id = T::ID;
        if (T::TYPE == Packet::Type::BOTH || T::TYPE == Packet::Type::CLIENTBOUND) {
            clientPackets[id] = packet<T>;
        }
        if (T::TYPE == Packet::Type::BOTH || T::TYPE == Packet::Type::SERVERBOUND) {
            serverPackets[id] = packet<T>;
        }
    }

public:
    static std::map<std::string, Protocol *> protocols;
    static void registerProtocol(Protocol *protocol) {
        protocols[StringUtils::toLowercase(protocol->getName())] = protocol;
    }
    static Protocol *getProtocol(std::string name) {
        if (protocols.count(name) > 0)
            return protocols[name];
        return nullptr;
    }

    Protocol() {}

    virtual std::string getName() = 0;

    virtual ProtocolServer &getServer() = 0;

    virtual Packet *getPacket(int id, bool client);
    virtual Packet *readPacket(BinaryStream &stream, bool client) = 0;
    virtual void writePacket(BinaryStream &stream, Packet &packet) = 0;

};
