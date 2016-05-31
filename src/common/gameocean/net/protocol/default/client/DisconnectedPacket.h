#pragma once

#include "gameocean/net/Packet.h"
#include "gameocean/net/Connection.h"
#include "gameocean/net/ConnectionHandler.h"
#include <string>

namespace DefaultProtocol {

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

        virtual void write(BinaryStream& stream) {
            stream << (int&) reason << textReason;
        };

        virtual void handleClient(Connection& connection) {
            connection.setAccepted(true);
            connection.getClientHandler().disconnected(reason, textReason);
        };

    };

}