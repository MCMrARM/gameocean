#pragma once

#include <gameocean/utils/Thread.h>

class MCPEProtocolServer;
class MCPEPacketBatchThread : public Thread {

protected:
    bool stopping = false;

public:
    MCPEProtocolServer& protocol;

    MCPEPacketBatchThread(MCPEProtocolServer& protocol) : protocol(protocol) { }

    virtual void run();
    virtual void stop() { stopping = true; };

};


