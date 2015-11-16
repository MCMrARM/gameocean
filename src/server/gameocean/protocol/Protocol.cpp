#include "Protocol.h"

#include "mcpe/MCPEProtocol.h"

std::map<std::string, Protocol*> Protocol::protocols = std::map<std::string, Protocol*>();

void Protocol::registerDefaultProtocols(Server& server) {
    registerProtocol(new MCPEProtocol(server));
}