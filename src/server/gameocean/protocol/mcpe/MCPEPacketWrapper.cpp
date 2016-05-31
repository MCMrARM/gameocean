#include "MCPEPacketWrapper.h"
#include "MCPEConnection.h"

void MCPEReceivePacketWrapper::read(BinaryStream &stream) {
    byte id;
    stream >> id;
    Logger::main->trace("MCPEPacket", "Received packet: %i", id);
    recvdPk = MCPEPacket::getPacket(id);
    if (recvdPk != nullptr) {
        recvdPk->read(stream);
    } else {
        Logger::main->warn("MCPEPacket", "Unknown packet: %i", id);
    }
}
void MCPEReceivePacketWrapper::handleServer(Connection &connection) {
    MCPEPlayer *player = ((MCPEConnection &) connection).getMCPEPlayer();
    if (player != nullptr)
        recvdPk->handle(*player);
}