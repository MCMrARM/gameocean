#pragma once


class Vector3D {

public:
    float x, y, z;

    Vector3D() : x(0), y(0), z(0) {};
    Vector3D(float x, float y, float z) : x(x), y(y), z(z) {};

    void add(Vector3D v) {
        x += v.x;
        y += v.y;
        z += v.z;
    }

};
