#include "ProtocolServer.h"

void ProtocolServer::loop() {
    while (true) {
        if (shouldStop)
            return;
        std::shared_ptr<Connection> connection (handleConnection());
        if (!connection)
            return;
        connection->setHandler(handler);
        new ConnectionReceiveThread(connection, true);
    }
}