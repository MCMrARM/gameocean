#include <iostream>
#include <gameocean/Server.h>
#include <gameocean/GameInfo.h>

int main() {
    GameInfo::current = new GameInfo("gameocean", Version(0, 0, 0, 1));
    GameInfo::current->version.setNetVersion(1, 1);
    Logger::main = new Logger();

    Server server;
    server.start();

    return 0;
}