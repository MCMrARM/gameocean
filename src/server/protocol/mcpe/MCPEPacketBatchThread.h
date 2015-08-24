#pragma once

#include "utils/Thread.h"

class MCPEProtocol;
class MCPEPacketBatchThread : public Thread {

protected:
    bool stopping = false;

public:
    MCPEProtocol& protocol;

    MCPEPacketBatchThread(MCPEProtocol& protocol) : protocol(protocol) { };

    virtual void run();
    virtual void stop() { stopping = true; };

};


