#pragma once

#include <string>
#include <ctime>

class Time {

public:
    /**
     * Returns the current time in milliseconds (since epoch).
     */
    static long long now();
    
    static inline std::tm getTimeStruct(long long time);
    
    static std::string getString(long long time, std::string format);
    
    /**
     * Returns current time as %H:%M:%S
     */
    static inline std::string getString(long long time) { return Time::getString(time, "%H:%M:%S"); };

};


