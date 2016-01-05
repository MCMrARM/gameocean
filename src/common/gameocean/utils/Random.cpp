#include "Random.h"

Random Random::instance;

Random::Random() : rd(), eng(rd()) {
    //
}

int Random::nextInt(int min, int max) {
    std::uniform_int_distribution<int> dist (min, max);
    return dist(eng);
}

float Random::nextFloat(float min, float max) {
    std::uniform_real_distribution<float> dist (min, max);
    return dist(eng);
}