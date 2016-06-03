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
    if (recvdPk == nullptr)
        return;
    std::shared_ptr<MCPEPlayer> player = ((MCPEConnection &) connection).getMCPEPlayer();
    if (player)
        recvdPk->handle(*player);
}