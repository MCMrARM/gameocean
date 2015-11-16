#pragma once

#include "../common.h"

template <size_t N>
class NibbleArray {

public:
    byte array [N];

    inline byte operator[](int n) {
        byte r = array[n / 2];
        if (n & 1) return (r >> 4);
        return (r & 0xf);
    };

    inline void set(int n, byte val) {
        byte r = array[n / 2];
        if (n & 1) { array[n / 2] = (r & 0xf) | ((val & 0xf) << 4); }
        else { array[n / 2] = (val & 0xf) | (r & 0xf0); }
    };

};


