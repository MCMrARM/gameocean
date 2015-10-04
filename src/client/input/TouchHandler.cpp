#include "TouchHandler.h"
#include "TouchEvent.h"
#include "../App.h"
#include "../gui/Screen.h"

bool TouchHandler::pressed [TouchHandler::MAX_TOUCHES];
int TouchHandler::x [TouchHandler::MAX_TOUCHES];
int TouchHandler::y [TouchHandler::MAX_TOUCHES];

void TouchHandler::reset() {
    for(int i = 0; i < TouchHandler::MAX_TOUCHES; i++) {
        TouchHandler::pressed[i] = false;
        TouchHandler::x[i] = -1;
        TouchHandler::y[i] = -1;
    }
}

void TouchHandler::press(int id, int x, int y) {
    if(id >= TouchHandler::MAX_TOUCHES) return;
    x /= App::instance->pixelSize;
    y = (App::instance->screenHeight - y) / App::instance->pixelSize;
    TouchHandler::pressed[id] = true;
    TouchHandler::x[id] = x;
    TouchHandler::y[id] = y;
    TouchPressEvent ev (x, y, id);
    std::shared_ptr<Screen> screen = App::instance->getScreen();
    if(screen) {
        screen->onMousePress(ev);
    }
}

void TouchHandler::release(int id, int x, int y) {
    if(id >= TouchHandler::MAX_TOUCHES) return;
    x /= App::instance->pixelSize;
    y = (App::instance->screenHeight - y) / App::instance->pixelSize;
    TouchHandler::pressed[id] = false;
    TouchReleaseEvent ev (x, y, id);
    std::shared_ptr<Screen> screen = App::instance->getScreen();
    if(screen) {
        screen->onMouseRelease(ev);
    }
}

void TouchHandler::move(int id, int x, int y) {
    if(id >= TouchHandler::MAX_TOUCHES) return;
    x /= App::instance->pixelSize;
    y = (App::instance->screenHeight - y) / App::instance->pixelSize;
    int dX = x - TouchHandler::x[id];
    int dY = y - TouchHandler::y[id];
    if(dX == 0 && dY == 0) return;

    TouchMoveEvent ev (x, y, dX, dY, id);
    TouchHandler::x[id] = x;
    TouchHandler::y[id] = y;
    std::shared_ptr<Screen> screen = App::instance->getScreen();
    if(screen) {
        screen->onMouseMove(ev);
    }
}