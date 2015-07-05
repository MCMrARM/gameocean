#pragma once

#include <string>

class StringUtils {

public:
    static std::string trim(std::string s) {
        if (s.length() <= 0) return "";
        int i = s.find_first_not_of(" ");
        return s.substr(i, s.find_last_not_of(" \r\n") + 1);
    };

};