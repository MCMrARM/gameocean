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
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    };

    static std::string toUppercase(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        return s;
    };

    static int compare(std::string s1, std::string s2) {
        int i;
        int l = std::min(s1.length(), s2.length());
        for (i = 0; i < l; i++) {
            if (tolower(s1[i]) != tolower(s2[i])) {
                break;
            }
        }
        return i;
    };

};