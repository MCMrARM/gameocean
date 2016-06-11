#include "RakNetPingThread.h"
#include "RakNetProtocolServer.h"

void RakNetPingThread::run() {
    while (true) {
        if (shouldStop)
            break;
        for (auto p : server.getConnections()) {
            if (!p.second->isMostLikelyTimeouted())
                p.second->sendPing();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(pingInterval));
    }
}