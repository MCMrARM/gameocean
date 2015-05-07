#include "Logger.h"

Logger* Logger::main = null;

void Logger::vlog(LogLevel level, const char *tag, const char *text, va_list args) {
    vsnprintf(buffer, BUFFER_SIZE, text, args);

    this->print(level, tag, buffer);
}

void Logger::log(LogLevel level, const char *tag, const char *text, ...) {
    va_list args;
    va_start(args, text);
    vlog(level, tag, text, args);
    va_end(args);
}

void Logger::trace(const char *tag, const char *text, ...) {
    va_list args;
    va_start(args, text);
    vlog(LogLevel::TRACE, tag, text, args);
    va_end(args);
}

void Logger::debug(const char *tag, const char *text, ...) {
    va_list args;
    va_start(args, text);
    vlog(LogLevel::DEBUG, tag, text, args);
    va_end(args);
}

void Logger::info(const char *tag, const char *text, ...) {
    va_list args;
    va_start(args, text);
    vlog(LogLevel::INFO, tag, text, args);
    va_end(args);
}

void Logger::warn(const char *tag, const char *text, ...) {
    va_list args;
    va_start(args, text);
    vlog(LogLevel::WARN, tag, text, args);
    va_end(args);
}

void Logger::error(const char *tag, const char *text, ...) {
    va_list args;
    va_start(args, text);
    vlog(LogLevel::ERROR, tag, text, args);
    va_end(args);
}

void MultiLogger::print(LogLevel level, const char *tag, const char *text) {
    for(Logger* logger : loggers) {
        logger->log(level, tag, text);
    }
}

void MultiLogger::addLogger(Logger* logger) {
    loggers.push_back(logger);
}