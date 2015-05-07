#pragma once

#include "utils/Logger.h"

class AndroidLogger : public Logger {

    virtual void print(LogLevel level, const char* tag, const char* text);

};