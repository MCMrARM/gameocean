#pragma once

#include "Version.h"

class GameInfo {

public:
    static GameInfo *current;

    std::string name;
    Version version;

    GameInfo(std::string name, Version version) : name(name), version(version) { }

};