#pragma once

#include <map>
#include "../Server.h"
#include "utils/Thread.h"
#include "utils/Logger.h"
#include "utils/StringUtils.h"

class Protocol : public Thread {

protected:
    Server& server;
    bool shouldStop = false;
    int port;

    virtual void run() {
        loop();
    };

public:
    static std::map<std::string, Protocol*> protocols;
    static void registerProtocol(Protocol* protocol) {
        protocols[StringUtils::toLowercase(protocol->getName())] = protocol;
    };
    static Protocol* getProtocol(std::string name) {
        if (protocols.count(name) > 0)
            return protocols[name];
        return null;
    };
    static void registerDefaultProtocols(Server& server);

    Protocol(Server& server) : server(server) {};

    virtual std::string getName() = 0;

    virtual void start() {
        Thread::start();
        Logger::main->debug("Protocol", "Starting %s protocol on port %i", getName().c_str(), port);
    }
    virtual void stop() {
        Logger::main->debug("Protocol", "Stopping %s protocol", getName().c_str());
        shouldStop = true;
    };

    virtual void loop() = 0;

    inline void setPort(int port) {
        if (isRunning())
            return;
        this->port = port;
    };
    inline int getPort() { return port; };

    virtual void setOption(std::string key, std::string value) {
        if (key == "port") {
            setPort(StringUtils::asInt(value, port));
        }
    };

};


