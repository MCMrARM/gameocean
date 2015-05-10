#include "iOSLogger.h"

//#include "CPPUtils.h"

void iOSLogger::print(LogLevel level, const char* tag, const char* text) {
    std::string l = "ERROR";
    if(level == LogLevel::LOG_TRACE) { l = "TRACE"; }
    else if(level == LogLevel::LOG_DEBUG) { l = "DEBUG"; }
    else if(level == LogLevel::LOG_INFO) { l = "INFO"; }
    else if(level == LogLevel::LOG_WARN) { l = "WARN"; }
    else if(level == LogLevel::LOG_ERROR) { l = "ERROR"; }
    
    NSLog(@"[%s] %s: %s", l.c_str(), tag, text);
}