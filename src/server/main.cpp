#include <iostream>
#include "GameInfo.h"
#include "utils/Logger.h"

int main() {
    GameInfo::current = new GameInfo("lbsg", Version(0, 0, 0, 1));
    GameInfo::current->version.setNetVersion(1, 1);
    Logger::main = new Logger();

    Logger::main->info("Main", "%s", (std::string("Game: ") + GameInfo::current->name + " v" + GameInfo::current->version.toString()).c_str());

    return 0;
}