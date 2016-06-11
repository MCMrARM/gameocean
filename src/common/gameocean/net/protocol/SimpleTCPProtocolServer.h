#pragma once

#include "ProtocolServer.h"

#include "../ServerSocket.h"

template <typename TConnection>
class SimpleTCPProtocolServer : public ProtocolServer {
protected:
    ServerSocket socket;

public:
    SimpleTCPProtocolServer(Protocol &protocol) : ProtocolServer(protocol) {
            //
    }

    virtual void loop() {
        if (!socket.listen("", (unsigned short) port, Socket::Protocol::TCP)) {
            Logger::main->error("SimpleTCPProtocolServer", "Failed to start listening");
            return;
        }
        ProtocolServer::loop();
        socket.close();
    }
    virtual Connection *handleConnection() {
        Socket *s = socket.accept();
        if (s == nullptr)
            return nullptr;
        return new TConnection(protocol, *s, false);
    }

};