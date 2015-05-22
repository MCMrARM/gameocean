#include "Time.h"

#include <chrono>

long long Time::now() {
    using namespace std::chrono;

    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return ms.count();
}