#pragma once

#include <string>

class KeyboardHandler {

public:
    static const int MAX_KEYS = 512;

public:
    static bool keys [MAX_KEYS];

    static void reset();

    static void press(int id);
    static void release(int id);

    // virtual keyboard
    static void inputSetText(std::string str);

};
