#pragma once

#include "../ProtocolServer.h"
#include "../../ServerSocket.h"

namespace DefaultProtocol {

    class SimpleTCPProtocolServer : public ProtocolServer {
    protected:
        ServerSocket socket;

    public:
        SimpleTCPProtocolServer(Protocol& protocol) : ProtocolServer(protocol) {
            //
        }

        virtual void loop();
        virtual Connection *handleConnection();

    };

}
