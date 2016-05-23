#include "SimpleTCPProtocolServer.h"
#include "SimpleTCPConnection.h"

namespace DefaultProtocol {

    void SimpleTCPProtocolServer::loop() {
        if (!socket.listen("", 1221, Socket::Protocol::TCP)) {
            Logger::main->error("SimpleTCPProtocolServer", "Failed to start listening");
            return;
        }
        ProtocolServer::loop();
        socket.close();
    }

    Connection *SimpleTCPProtocolServer::handleConnection() {
        Socket *s = socket.accept();
        if (s == nullptr)
            return nullptr;
        return new SimpleTCPConnection(protocol, *s, false);
    }

}