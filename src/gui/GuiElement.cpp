#include "GuiElement.h"

void GuiElement::setPosition(int x, int y) {
    this->x = x;
    this->y = y;
    this->requireUpdate();
    this->posUpdate = true;
}

void GuiElement::setSize(int w, int h) {
    this->width = w;
    this->height = h;
    this->requireUpdate();
    this->posUpdate = true;
}