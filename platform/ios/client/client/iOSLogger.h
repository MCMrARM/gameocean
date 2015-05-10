#pragma once

#include <string>
#include "Logger.h"

class iOSLogger : public Logger {
    
    virtual void print(LogLevel level, const char* tag, const char* text);
    
};