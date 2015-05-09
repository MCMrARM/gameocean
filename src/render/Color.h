#pragma once


class Color {

public:

    float r, g, b, a;

    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {};
    Color(float r, float g, float b) : Color(r, g, b, 1) {};
    Color(int r, int g, int b, int a) : r((float) r / 255), g((float) g / 255), b((float) b / 255) {};
    Color(int r, int g, int b) : Color(r, g, b, 255) {};

};


