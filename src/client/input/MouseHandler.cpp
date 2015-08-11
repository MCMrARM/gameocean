#include "MouseHandler.h"
#include "MouseEvent.h"
#include "../App.h"
#include "../gui/Screen.h"
#include "utils/Logger.h"

int MouseHandler::x = -1;
int MouseHandler::y = -1;
bool MouseHandler::buttonsPressed [MouseHandler::MAX_BUTTONS];

void MouseHandler::reset() {
    for(int i = 0; i < MAX_BUTTONS; i++) {
        MouseHandler::buttonsPressed[i] = false;
    }
    MouseHandler::x = -1;
    MouseHandler::y = -1;
}

void MouseHandler::press(int button) {
    if(button >= MouseHandler::MAX_BUTTONS) return;
    MouseHandler::buttonsPressed[button] = true;
    MousePressEvent ev (MouseHandler::x, MouseHandler::y, button);
    Screen* screen = App::instance->getScreen();
    if(screen != null) {
        screen->onMousePress(ev);
    }
}

void MouseHandler::release(int button) {
    if(button >= MouseHandler::MAX_BUTTONS) return;
    MouseHandler::buttonsPressed[button] = false;
    MouseReleaseEvent ev (MouseHandler::x, MouseHandler::y, button);
    Screen* screen = App::instance->getScreen();
    if(screen != null) {
        screen->onMouseRelease(ev);
    }
}

void MouseHandler::move(int x, int y) {
    x /= App::instance->pixelSize;
    y = (App::instance->screenHeight - y) / App::instance->pixelSize;

    int dX = x - MouseHandler::x;
    int dY = y - MouseHandler::y;
    if(dX == 0 && dY == 0) return;

    MouseMoveEvent ev (x, y, dX, dY);
    MouseHandler::x = x;
    MouseHandler::y = y;
    Screen* screen = App::instance->getScreen();
    if(screen != null) {
        screen->onMouseMove(ev);
    }
}