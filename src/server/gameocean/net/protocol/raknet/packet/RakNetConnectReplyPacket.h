#pragma once

#include <gameocean/net/Packet.h>
#include "../RakNetPacketIds.h"
#include "../RakNetProtocol.h"

struct RakNetConnectReplyPacket : public Packet {
    PacketDef(RAKNET_PACKET_CONNECT_REPLY, Type::CLIENTBOUND)

    bool protocolMagicCorrect = true;
    long long serverId;
    sockaddr clientAddr;
    short mtu;
    long long clientId;
    byte encryption = 0;

    RakNetConnectReplyPacket() {
        memset(&clientAddr, 0, sizeof(clientAddr));
    }

    virtual unsigned int getPacketSize() const {
        return sizeof(RakNetPacketMagic) + BinaryStream::LONG_SIZE + 2 * BinaryStream::SHORT_SIZE + BinaryStream::LONG_SIZE;
    }

    virtual void read(BinaryStream& stream) {
        protocolMagicCorrect = RakNetProtocol::checkRakNetMagicBytes(stream);
        stream >> serverId;
        clientAddr = RakNetProtocol::readRakNetAddress(stream);
        stream >> mtu >> clientId >> encryption;
    }

    virtual void write(BinaryStream& stream) {
        stream.write(RakNetPacketMagic, sizeof(RakNetPacketMagic));
        stream << serverId;
        RakNetProtocol::writeRakNetAddress(stream, clientAddr);
        stream << mtu << clientId << encryption;
    }
};