#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

class StringUtils {

public:
    static std::string trim(std::string s) {
        if (s.length() <= 0) return "";
        int i = s.find_first_not_of(" ");
        return s.substr(i, s.find_last_not_of(" \r\n") + 1);
    };

    static std::vector<std::string> split(std::string s, std::string d) {
        std::vector<std::string> v;
        std::size_t p = 0;
        while (p < s.length()) {
            int i = s.find_first_of(d, p);
            if (i == std::string::npos) {
                v.push_back(s.substr(p));
                break;
            }
            v.push_back(s.substr(p, i - p));
            p = i + d.size();
        }
        return v;
    };

    static std::string toLowercase(std::string s) {
        std::cout << s << std::endl;
        //std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        //std::cout << s << std::endl;
        return s;
    };

    static std::string toUppercase(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        return s;
    };

};