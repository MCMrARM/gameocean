#include <iostream>
#include "Server.h"
#include "GameInfo.h"
#include "utils/Logger.h"
#include "utils/Config.h"

int main() {
    GameInfo::current = new GameInfo("gameocean", Version(0, 0, 0, 1));
    GameInfo::current->version.setNetVersion(1, 1);
    Logger::main = new Logger();

    Server server;
    server.start();

    return 0;
}