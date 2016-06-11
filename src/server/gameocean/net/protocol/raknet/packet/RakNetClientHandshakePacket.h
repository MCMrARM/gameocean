#pragma once

#include <gameocean/net/Packet.h>
#include "../RakNetPacketIds.h"
#include "../RakNetProtocol.h"

struct RakNetClientHandshakePacket : public Packet {
    PacketDef(RAKNET_PACKET_CLIENT_HANDSHAKE, Type::SERVERBOUND)

    sockaddr addr;
    sockaddr internalIDs[10];
    long long requestTime, time;

    RakNetClientHandshakePacket() {
        memset(&addr, 0, sizeof(addr));
        memset(&internalIDs, 0, sizeof(internalIDs));
        for (int i = 0; i < 10; i++) {
            internalIDs[i].sa_family = AF_INET;
            ((sockaddr_in&) internalIDs[i]).sin_addr.s_addr = 0xffffffff;
            ((sockaddr_in&) internalIDs[i]).sin_port = 0;
        }
    }

    virtual unsigned int getPacketSize() const {
        unsigned int idsSize = 0;
        for (int i = 0; i < 10; i++)
            idsSize += RakNetProtocol::getRakNetAddressSize(internalIDs[i]);
        return RakNetProtocol::getRakNetAddressSize(addr) + idsSize + 2 * BinaryStream::LONG_SIZE;
    }

    virtual void read(BinaryStream& stream) {
        addr = RakNetProtocol::readRakNetAddress(stream);
        for (int i = 0; i < 10; i++)
            internalIDs[i] = RakNetProtocol::readRakNetAddress(stream);
        stream >> requestTime >> time;
    }

    virtual void write(BinaryStream& stream) {
        RakNetProtocol::writeRakNetAddress(stream, addr);
        for (int i = 0; i < 10; i++)
            RakNetProtocol::writeRakNetAddress(stream, internalIDs[i]);
        stream << requestTime << time;
    }

    virtual void handleServer(Connection &connection);
};