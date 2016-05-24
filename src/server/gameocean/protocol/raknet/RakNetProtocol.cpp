#include "RakNetProtocol.h"
#include "RakNetPacketIds.h"
#include <unistd.h>

Packet *RakNetProtocol::readPacket(BinaryStream &stream, bool client) {
    byte data[1500-20-8];
    ssize_t pkLen = read(((FileBinaryStream*) &stream)->getFileDescriptor(), data, sizeof(data));

    MemoryBinaryStream pkStream(data, (unsigned int) pkLen);
    unsigned char pkId;
    pkStream >> pkId;
    Logger::main->trace("RakNetProtocol", "Received packet with id: %i", pkId);

    Packet* pk = getPacket(pkId, client);
    if (pk == nullptr) {
        Logger::main->warn("RakNetProtocol", "Unknown packet received: %i, size: %i", pkId, pkLen);
        pk = new UnknownPacket(pkId);
    }
    pk->read(pkStream);
    return pk;
}
void RakNetProtocol::writePacket(BinaryStream &stream, const Packet &packet) {
    int bufLen = BinaryStream::BYTE_SIZE + packet.getPacketSize();

    byte buf[bufLen];
    MemoryBinaryStream pkStream(buf, bufLen);

    char pkId = (char) packet.getId();
    pkStream << pkId;
    packet.write(pkStream);

    stream.write(buf, (unsigned int) bufLen);
}

bool RakNetProtocol::checkRakNetMagicBytes(char magic[16]) {
    return (memcmp(magic, RakNetPacketMagic, 16) == 0);
}
bool RakNetProtocol::checkRakNetMagicBytes(BinaryStream &stream) {
    char magic[16];
    stream.readFully((byte*) magic, 16);
    return checkRakNetMagicBytes(magic);
}