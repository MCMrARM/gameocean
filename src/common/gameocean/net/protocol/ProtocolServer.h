#pragma once

#include "../../utils/Thread.h"
#include "Protocol.h"
#ifdef SERVER
class Server;
#endif

class Connection;

class ProtocolServer : public Thread {

protected:
#ifdef SERVER
    Server *server = nullptr;
#endif
    Protocol &protocol;
    ConnectionHandler *handler = nullptr;
    int port;
    bool shouldStop = false;

    virtual void run() {
        loop();
    }

public:
    ProtocolServer(Protocol &protocol) : protocol(protocol) {
        //
    }

#ifdef SERVER
    inline Server *getServer() {
        return server;
    }
#endif

    inline Protocol &getProtocol() {
        return protocol;
    }

    inline ConnectionHandler *getHandler() {
        return handler;
    }
    inline void setHandler(ConnectionHandler *handler) {
        this->handler = handler;
    }

    virtual void start() {
        Thread::start();
        Logger::main->debug("ProtocolServer", "Started %s protocol on port %i", protocol.getName().c_str(), port);
    }
#ifdef SERVER
    virtual void start(Server* server) {
        this->server = server;
        start();
    }
#endif
    virtual void stop() {
        Logger::main->debug("ProtocolServer", "Stopping %s protocol", protocol.getName().c_str());
        shouldStop = true;
    }

    virtual void loop();
    virtual Connection* handleConnection() = 0;

    inline void setPort(int port) {
        if (isRunning())
            return;
        this->port = port;
    }
    inline int getPort() { return port; }

    virtual void setOption(std::string key, std::string value) {
        if (key == "port")
            setPort(StringUtils::asInt(value, port));
    }

};

