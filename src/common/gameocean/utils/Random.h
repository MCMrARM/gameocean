#pragma once

#include <random>

class Random {

private:
    std::random_device rd;
    std::default_random_engine eng;

public:
    static Random instance;

    Random();

    int nextInt(int min, int max);

    float nextFloat(float min, float max);
    inline float nextFloat() {
        return nextFloat(0.f, 1.f);
    }

};


