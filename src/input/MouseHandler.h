#pragma once

class MouseHandler {

public:
    static const int MAX_BUTTONS = 32;

    static bool buttonsPressed [MAX_BUTTONS];
    static int x;
    static int y;

    static void reset();

    static void press(int button);
    static void release(int button);
    static void move(int x, int y);

};


