#include "SimpleTCPProtocol.h"
#include "server/ConnectPacket.h"
#include "client/ConnectedPacket.h"
#include "client/DisconnectedPacket.h"

namespace DefaultProtocol {

    SimpleTCPProtocol::SimpleTCPProtocol() : server(*this) {
        registerPacket<ConnectPacket>();
        registerPacket<ConnectedPacket>();
        registerPacket<DisconnectedPacket>();
    }

    Packet* SimpleTCPProtocol::readPacket(BinaryStream& stream, bool client) {
        int pkLen;
        stream >> pkLen;
        Logger::main->trace("SimpleTCPProtocol", "Received packet with size: %i", pkLen);

        byte packet[pkLen];
        stream.read(packet, pkLen);

        MemoryBinaryStream pkStream(packet, pkLen);
        unsigned char pkId;
        pkStream >> pkId;
        Logger::main->trace("SimpleTCPProtocol", "Received packet with id: %i", pkId);

        Packet* pk = getPacket(pkId, client);
        if (pk == nullptr) {
            Logger::main->warn("SimpleTCPProtocol", "Unknown packet received: %i, size: %i", pkId, pkLen);
            pk = new UnknownPacket(pkId);
        }
        pk->read(pkStream);
        return pk;
    }

    void SimpleTCPProtocol::writePacket(BinaryStream& stream, Packet& packet) {
        int pkLen = BinaryStream::BYTE_SIZE + packet.getPacketSize();
        int bufLen = BinaryStream::INT_SIZE + pkLen;

        byte buf[bufLen];
        MemoryBinaryStream pkStream(buf, bufLen);

        char pkId = (char) packet.getId();
        pkStream << pkLen << pkId;
        packet.write(pkStream);

        stream.write(buf, bufLen);
    }

}