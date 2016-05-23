#pragma once

#include "../Protocol.h"
#include "SimpleTCPProtocolServer.h"

namespace DefaultProtocol {

    class SimpleTCPProtocol : public Protocol {

    private:
        SimpleTCPProtocolServer server;

    public:
        SimpleTCPProtocol();

        virtual Packet *readPacket(BinaryStream &stream, bool client);
        virtual void writePacket(BinaryStream &stream, const Packet &packet);

        virtual std::string getName() { return "default-tcp"; }

        virtual ProtocolServer& getServer() {
            return server;
        }

    };

}

