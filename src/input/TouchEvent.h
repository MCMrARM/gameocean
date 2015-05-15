#include "MouseEvent.h"

struct TouchMoveEvent : public MouseMoveEvent {

    int fingerId;

    TouchMoveEvent(int x, int y, int dX, int dY, int fingerId) : MouseMoveEvent(x, y, dX, dY), fingerId(fingerId) {};

    virtual bool isTouch() { return true; }

};

struct TouchPressEvent : public MousePressEvent {

    int fingerId;

    TouchPressEvent(int x, int y, int fingerId) : MousePressEvent(x, y, 0), fingerId(fingerId) {};

    virtual bool isTouch() { return true; }

};

struct TouchReleaseEvent : public MouseReleaseEvent {

    int fingerId;

    TouchReleaseEvent(int x, int y, int fingerId) : MouseReleaseEvent(x, y, 0), fingerId(fingerId) {};

    virtual bool isTouch() { return true; }

};