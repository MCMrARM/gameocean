#pragma once

class TouchHandler {

public:
    static const int MAX_TOUCHES = 10;

public:
    static bool pressed [MAX_TOUCHES];
    static int x [MAX_TOUCHES];
    static int y [MAX_TOUCHES];

    static void reset();

    static void press(int id, int x, int y);
    static void release(int id, int x, int y);
    static void move(int id, int x, int y);

};


