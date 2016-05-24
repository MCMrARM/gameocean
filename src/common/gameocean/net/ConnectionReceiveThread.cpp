#include "ConnectionReceiveThread.h"
#include "Connection.h"

void ConnectionReceiveThread::run() {
    connection->loop();
}