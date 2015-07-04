#pragma once

#include "../common.h"
#include <string>
#include <vector>

enum class LogLevel {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

class Logger {

    static const int BUFFER_SIZE = 4096;

    char buffer [BUFFER_SIZE];
    
    static inline std::string getLogLevelString(LogLevel lvl) {
        if (lvl == LogLevel::LOG_TRACE) return "Trace";
        if (lvl == LogLevel::LOG_DEBUG) return "Debug";
        if (lvl == LogLevel::LOG_INFO) return "Info";
        if (lvl == LogLevel::LOG_WARN) return "Warn";
        if (lvl == LogLevel::LOG_ERROR) return "Error";
        return "?";
    }

    void vlog(LogLevel level, const char* tag, const char* text, va_list args);

    virtual void print(LogLevel level, const char* tag, const char* text);

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