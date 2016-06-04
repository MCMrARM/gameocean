#include "RakNetResendThread.h"
#include "RakNetProtocolServer.h"

void RakNetResendThread::run() {
    while (true) {
        if (shouldStop)
            break;
        for (auto p : server.getConnections()) {
            p.second->resendPackets();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(resendDelay));
    }
}