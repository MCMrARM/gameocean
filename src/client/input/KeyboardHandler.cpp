#include "KeyboardHandler.h"

#include "../App.h"
#include "../gui/Screen.h"

bool KeyboardHandler::keys [KeyboardHandler::MAX_KEYS];

void KeyboardHandler::reset() {
    for(int i = 0; i < KeyboardHandler::MAX_KEYS; i++) {
        KeyboardHandler::keys[i] = false;
    }
}

void KeyboardHandler::press(int id) {
    if(id >= MAX_KEYS) return;
    KeyboardHandler::keys[id] = true;
}

void KeyboardHandler::release(int id) {
    if(id >= MAX_KEYS) return;
    KeyboardHandler::keys[id] = false;
}

void KeyboardHandler::inputSetText(std::string str) {
    std::shared_ptr<Screen> screen = App::instance->getScreen();
    if(screen) {
        screen->setText(str);
    }
}