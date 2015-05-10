#pragma once

#include "App.h"

class iOSApp : public App {
    
public:
    virtual MultiLogger* initLogger();
    virtual void destroyOpenGL();
    virtual byte* readGameFile(std::string name, unsigned int& size);
    
};
