#pragma once

#include "../../../../GameInfo.h"
#include "../../../Network.h"
#include "../../../Packet.h"
#include "../../../Connection.h"
#include "../../../ConnectionHandler.h"
#include "../client/ConnectedPacket.h"
#include "../client/DisconnectedPacket.h"
#include <string>
#include <gameocean/Version.h>

namespace DefaultProtocol {

    class ConnectPacket : public Packet {

    public:
        static const int ID = 0;
        static const Type TYPE = Type::SERVERBOUND;

        long long protocolMagic = Network::PROTOCOL_MAGIC;
        int protcolVersion = Network::PROTOCOL_VERSION;
        std::string gameName;
        int gameVersion;

        ConnectPacket() {
            gameName = GameInfo::current->name;
            gameVersion = GameInfo::current->version.netVersion;
        };

        virtual int getId() const { return ID; };

        virtual Type getType() { return TYPE; };

        virtual unsigned int getPacketSize() const {
            return BinaryStream::LONG_SIZE + 2 * BinaryStream::INT_SIZE + BinaryStream::getStringSize(gameName);
        };

        virtual void read(BinaryStream& stream) {
            stream >> protocolMagic >> protcolVersion >> gameName >> gameVersion;
        };

        virtual void write(BinaryStream& stream) const {
            stream << protocolMagic << protcolVersion << gameName << gameVersion;
        };

        virtual void handleServer(Connection& connection) {
            if (protocolMagic != Network::PROTOCOL_MAGIC) {
                connection.close(Connection::DisconnectReason::CLOSED);
                return;
            }

            if (gameName != GameInfo::current->name) {
                connection.close(Connection::DisconnectReason::WRONG_GAME);
                return;
            }

            if (protcolVersion < Network::MIN_PROTOCOL_VERSION) {
                connection.close(Connection::DisconnectReason::OUTDATED_CLIENT);
                return;
            }
            if (protcolVersion > Network::PROTOCOL_VERSION) {
                connection.close(Connection::DisconnectReason::OUTDATED_SERVER);
                return;
            }

            if (gameVersion < GameInfo::current->version.minNetVersion) {
                connection.close(Connection::DisconnectReason::OUTDATED_GAME);
                return;
            }
            if (gameVersion > GameInfo::current->version.netVersion) {
                connection.close(Connection::DisconnectReason::OUTDATED_GAME_SERVER);
                return;
            }

            ServerConnectionHandler::ClientAcceptanceStatus status = connection.getServerHandler().acceptClient(
                    connection);
            if (!status.accepted) {
                connection.kick(status.reason);
                return;
            }
            connection.setAccepted(true);

            ConnectedPacket pk;
            connection.send(pk);
        };

    };

}