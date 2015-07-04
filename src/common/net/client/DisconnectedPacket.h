#pragma once

#include "../Packet.h"
#include "../Connection.h"
#include "../ConnectionHandler.h"
#include <string>

class DisconnectedPacket : public Packet {

public:
    static const int ID = 1;
    static const Type TYPE = Type::CLIENTBOUND;

    Connection::DisconnectReason reason;
    std::string textReason;

    virtual int getId() const { return ID; };
    virtual Type getType() { return TYPE; };

    virtual unsigned int getPacketSize() const {
        return BinaryStream::INT_SIZE + BinaryStream::getStringSize(textReason);
    };
    virtual void read(BinaryStream& stream) {
        stream >> (int&) reason >> textReason;
    };
    virtual void write(BinaryStream& stream) const {
        stream << (int&) reason << textReason;
    };

    virtual void handleClient(Connection &connection) {
        connection.accepted = true;
        connection.getClientHandler().disconnected(reason, textReason);
    };

};