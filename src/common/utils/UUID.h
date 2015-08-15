#pragma once

#include <string>
#include <sstream>
#include <iomanip>

class UUID {

public:
    long long part1, part2;

    operator std::string() const {
        std::stringstream ss;
        ss << std::setfill ('0') << std::setw(16) << std::hex << part1 << part2;
        return ss.str();
    }

};


