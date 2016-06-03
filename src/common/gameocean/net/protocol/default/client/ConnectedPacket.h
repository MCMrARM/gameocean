#pragma once

#include "gameocean/Version.h"
#include "gameocean/net/Packet.h"
#include "gameocean/net/Connection.h"
#include "gameocean/net/ConnectionHandler.h"
#include <string>

namespace DefaultProtocol {

    class ConnectedPacket : public Packet {

    public:
        static const int ID = 0;
        static const Type TYPE = Type::CLIENTBOUND;

        long long protcolMagic = Network::PROTOCOL_MAGIC;
        int protocolVersion = Network::PROTOCOL_VERSION;
        int gameVersion;

        ConnectedPacket() {
            gameVersion = GameInfo::current->version.netVersion;
        };

        virtual int getId() const { return ID; };

        virtual Type getType() { return TYPE; };

        virtual unsigned int getPacketSize() const {
            return BinaryStream::LONG_SIZE + 2 * BinaryStream::INT_SIZE;
        };

        virtual void read(BinaryStream& stream) {
            stream >> protcolMagic >> protocolVersion >> gameVersion;
        };

        virtual void write(BinaryStream& stream) {
            stream << protcolMagic << protocolVersion << gameVersion;
        };

        virtual void handleClient(Connection& connection) {
            connection.setAccepted(true);
        };

    };

}