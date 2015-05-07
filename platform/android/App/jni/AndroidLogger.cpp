#include <android/log.h>
#include "AndroidLogger.h"

void AndroidLogger::print(LogLevel level, const char *tag, const char *text) {
    int l = ANDROID_LOG_ERROR;
    if(level == LogLevel::TRACE) { l = ANDROID_LOG_VERBOSE; }
    else if(level == LogLevel::DEBUG) { l = ANDROID_LOG_DEBUG; }
    else if(level == LogLevel::INFO) { l = ANDROID_LOG_INFO; }
    else if(level == LogLevel::WARN) { l = ANDROID_LOG_WARN; }
    else if(level == LogLevel::ERROR) { l = ANDROID_LOG_ERROR; }

    __android_log_print(l, tag, "%s", text);
}