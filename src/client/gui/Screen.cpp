#include "Screen.h"
#include "../App.h"
#include "../anim/Animation.h"

Screen::Screen(App *app) {
    this->x = 0;
    this->y = 0;
    this->width = app->guiWidth;
    this->height = app->guiHeight;
    this->clip = false;
}

void Screen::render() {
    for (auto it = animations.begin(); it != animations.end(); ) {
        if ((*it)->tick()) {
            it = animations.erase(it);
        } else {
            it++;
        }
    }

    GuiElementContainer::render();
}