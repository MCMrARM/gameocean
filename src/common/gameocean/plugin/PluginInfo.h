#pragma once

#include <string>
#include <vector>

class PluginInfo {

public:
    void* handle;
    std::string name;
    std::string dirName;
    std::string libFile;

};