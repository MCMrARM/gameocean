#include "ProtocolServer.h"

void ProtocolServer::loop() {
    while (true) {
        if (shouldStop)
            return;
        handleConnection();
    }
}