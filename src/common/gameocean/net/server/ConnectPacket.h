#pragma once

#include "../../GameInfo.h"
#include "../Network.h"
#include "../Packet.h"
#include "../Connection.h"
#include "../ConnectionHandler.h"
#include "../client/ConnectedPacket.h"
#include "../client/DisconnectedPacket.h"
#include <string>
#include <gameocean/Version.h>

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

    virtual void read(BinaryStream &stream) {
        stream >> protocolMagic >> protcolVersion >> gameName >> gameVersion;
    };

    virtual void write(BinaryStream &stream) const {
        stream << protocolMagic << protcolVersion << gameName << gameVersion;
    };

    virtual void handleServer(Connection &connection) {
        if (protocolMagic != Network::PROTOCOL_MAGIC) {
            connection.close();
            return;
        }

        if (gameName != GameInfo::current->name) {
            DisconnectedPacket pk;
            pk.reason = Connection::DisconnectReason::WRONG_GAME;
            Packet::sendPacket(connection, pk);
            connection.close();
            return;
        }

        if (protcolVersion < Network::MIN_PROTOCOL_VERSION) {
            DisconnectedPacket pk;
            pk.reason = Connection::DisconnectReason::OUTDATED_CLIENT;
            Packet::sendPacket(connection, pk);
            connection.close();
            return;
        }
        if (protcolVersion > Network::PROTOCOL_VERSION) {
            DisconnectedPacket pk;
            pk.reason = Connection::DisconnectReason::OUTDATED_SERVER;
            Packet::sendPacket(connection, pk);
            connection.close();
            return;
        }

        if (gameVersion < GameInfo::current->version.minNetVersion) {
            DisconnectedPacket pk;
            pk.reason = Connection::DisconnectReason::OUTDATED_GAME;
            Packet::sendPacket(connection, pk);
            connection.close();
            return;
        }
        if (gameVersion > GameInfo::current->version.netVersion) {
            DisconnectedPacket pk;
            pk.reason = Connection::DisconnectReason::OUTDATED_GAME_SERVER;
            Packet::sendPacket(connection, pk);
            connection.close();
            return;
        }

        ServerConnectionHandler::ClientAcceptanceStatus status = connection.getServerHandler().acceptClient(connection);
        if (!status.accepted) {
            DisconnectedPacket pk;
            pk.reason = Connection::DisconnectReason::KICKED;
            pk.textReason = status.reason;
            Packet::sendPacket(connection, pk);
            connection.close();
            return;
        }
        connection.accepted = true;

        ConnectedPacket pk;
        Packet::sendPacket(connection, pk);
    };

};