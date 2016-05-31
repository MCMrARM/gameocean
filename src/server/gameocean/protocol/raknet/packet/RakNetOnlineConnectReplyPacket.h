#pragma once

#include <gameocean/net/Packet.h>
#include "../RakNetPacketIds.h"
#include "../RakNetProtocol.h"

struct RakNetOnlineConnectReplyPacket : public Packet {
    PacketDef(RAKNET_PACKET_ONLINE_CONNECT_REPLY, Type::CLIENTBOUND)

    sockaddr clientAddr;
    short systemIndex = 0;
    sockaddr internalIDs[10];
    long long requestTime, time;

    RakNetOnlineConnectReplyPacket() {
        memset(&clientAddr, 0, sizeof(clientAddr));
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
        return RakNetProtocol::getRakNetAddressSize(clientAddr) + BinaryStream::SHORT_SIZE + idsSize + 2 * BinaryStream::LONG_SIZE;
    }

    virtual void read(BinaryStream& stream) {
        clientAddr = RakNetProtocol::readRakNetAddress(stream);
        stream >> systemIndex;
        for (int i = 0; i < 10; i++)
            internalIDs[i] = RakNetProtocol::readRakNetAddress(stream);
        stream >> requestTime >> time;
    }

    virtual void write(BinaryStream& stream) {
        RakNetProtocol::writeRakNetAddress(stream, clientAddr);
        stream << systemIndex;
        for (int i = 0; i < 10; i++)
            RakNetProtocol::writeRakNetAddress(stream, internalIDs[i]);
        stream << requestTime << time;
    }
};