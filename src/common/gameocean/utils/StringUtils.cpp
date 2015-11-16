#include "StringUtils.h"

#include <stdarg.h>

std::string StringUtils::sprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer [4096];
    vsprintf(&buffer[0], format, args);
    va_end(args);
    return std::string(buffer);
}