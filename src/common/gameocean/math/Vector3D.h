#pragma once

#include <cmath>

class Vector3D {

public:
    float x, y, z;

    Vector3D() : x(0), y(0), z(0) { }
    Vector3D(float x, float y, float z) : x(x), y(y), z(z) { }

    void add(Vector3D v) {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    float distance() {
        return std::sqrt(x * x + y * y + z * z);
    }

    void normalize() {
        float d = distance();
        if (d != 0.f) {
            x /= d;
            y /= d;
            z /= d;
        }
    }

};
