#pragma once

#include <streambuf>

struct membuf : std::streambuf
{
    membuf(char* begin, char* end) {
        this->setg(begin, begin, end);
    }
};