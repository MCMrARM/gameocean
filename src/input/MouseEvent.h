#pragma once

#include "InputEvent.h"

struct MouseEvent : public InputEvent {

    int x, y;

    MouseEvent(int x, int y) : x(x), y(y) {};

    virtual bool isTouch() { return false; }

};

struct MouseMoveEvent : public MouseEvent {

    int dX, dY;

    MouseMoveEvent(int x, int y, int dX, int dY) : MouseEvent(x, y), dX(dX), dY(dY) {};

};

struct MousePressEvent : public MouseEvent {

    int buttonId;

    MousePressEvent(int x, int y, int buttonId) : MouseEvent(x, y), buttonId(buttonId) {};

};

struct MouseReleaseEvent : public MouseEvent {

    int buttonId;

    MouseReleaseEvent(int x, int y, int buttonId) : MouseEvent(x, y), buttonId(buttonId) {};

};