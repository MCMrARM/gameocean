#include "Time.h"

#include <chrono>
#include <ctime>

long long Time::now() {
    using namespace std::chrono;

    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return ms.count();
}

std::tm Time::getTimeStruct(long long time) {
    time /= 1000;
    std::tm tm;
    tm.tm_sec = (int) (time % 60);
    time /= 60;
    tm.tm_min = (int) (time % 60);
    time /= 60;
    tm.tm_hour = (int) (time % 24);
    time /= 24;
    tm.tm_mday = (int) ((time % 31) + 1);
    time /= 31;
    tm.tm_mon = (int) (time % 12);
    time /= 12;
    tm.tm_year = (int) time;
    tm.tm_wday = 0;
    tm.tm_yday = 0;
    tm.tm_isdst = -1;
    return tm;
}

std::string Time::getString(long long time, std::string format) {
    char b [127];
    std::tm tm = Time::getTimeStruct(time);
    std::size_t s = std::strftime(b, 127, format.c_str(), &tm);
    return std::string(b, s);
}