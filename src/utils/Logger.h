#pragma once

#include "../common.h"
#include <string>
#include <vector>

enum class LogLevel {
    TRACE, DEBUG, INFO, WARN, ERROR
};

class Logger {

    static const int BUFFER_SIZE = 4096;

    char buffer [BUFFER_SIZE];

    void vlog(LogLevel level, const char* tag, const char* text, va_list args);

    virtual void print(LogLevel level, const char* tag, const char* text) = 0;

public:

    static Logger* main;

    void log(LogLevel level, const char* tag, const char* text, ...);

    void trace(const char* tag, const char* text, ...);
    void debug(const char* tag, const char* text, ...);
    void info(const char* tag, const char* text, ...);
    void warn(const char* tag, const char* text, ...);
    void error(const char* tag, const char* text, ...);

};

class MultiLogger : public Logger {

    virtual void print(LogLevel level, const char* tag, const char* text);

public:

    std::vector<Logger*> loggers;

    void addLogger(Logger* logger);

};