#pragma once

#include "../Packet.h"
#include "../Connection.h"
#include "../ConnectionHandler.h"
#include <string>

class ConnectedPacket : public Packet {

public:
    static const int ID = 0;
    static const Type TYPE = Type::CLIENTBOUND;

    long long protcolMagic = Network::PROTOCOL_MAGIC;
    int protocolVersion = Network::PROTOCOL_VERSION;
    int gameVersion = Network::GAME_VERSION;

    virtual int getId() const { return ID; };
    virtual Type getType() { return TYPE; };

    virtual unsigned int getPacketSize() const {
        return BinaryStream::LONG_SIZE + 2 * BinaryStream::INT_SIZE;
    };
    virtual void read(BinaryStream& stream) {
        stream >> protcolMagic >> protocolVersion >> gameVersion;
    };
    virtual void write(BinaryStream& stream) const {
        stream << protcolMagic << protocolVersion << gameVersion;
    };

    virtual void handleClient(Connection &connection) {
        connection.accepted = true;
        connection.getClientHandler().connected();
    };

};