#pragma once

#include <netdb.h>

struct Datagram {
    bool valid = true;
    sockaddr_in addr;
    char data[65507];
    ssize_t dataSize;
};