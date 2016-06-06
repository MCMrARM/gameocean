#include "StringUtils.h"

#include <stdarg.h>

std::string StringUtils::sprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buffer [4096];
    vsprintf(&buffer[0], format, args);
    va_end(args);
    return std::string(buffer);
}

void StringUtils::printInHex(std::ostream &stream, unsigned char *data, size_t dataSize) {
    static const char *charMap = "0123456789abcdef";
    for (size_t off = 0; off < dataSize; off += 16) {
        size_t off2max = std::min(off + 16, dataSize);
        for (size_t off2 = off; off2 < off2max; off2++) {
            stream << charMap[(data[off2] / 16) % 16];
            stream << charMap[data[off2] % 16];
            stream << ' ';
        }
        for (size_t off2 = off2max; off2 < off + 16; off2++) {
            stream << "   ";
        }
        stream << " ";
        for (size_t off2 = off; off2 < off2max; off2++) {
            char c = data[off2];
            if (c == ' ') {
                stream << '.';
            } else if (c > ' ' && c <= '~') { // printable chars
                stream << c;
            } else {
                stream << '?';
            }
        }
        stream << std::endl;
    }
}