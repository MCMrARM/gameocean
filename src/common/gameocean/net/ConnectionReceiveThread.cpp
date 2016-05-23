#include "ConnectionReceiveThread.h"
#include "Connection.h"

void ConnectionReceiveThread::run() {
    while (true) {
        if (!connection.readAndHandlePacket())
            return;
    }
}

void ConnectionReceiveThread::stop() {
    // TODO:
}