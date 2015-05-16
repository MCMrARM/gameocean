#include "GuiButtonElement.h"

#include "../common.h"

GuiButtonElement::GuiButtonElement(int x, int y, int w, int h) : GuiNinePathImageElement(x, y, w, h, null, 0, 0, 8, 8, 2) {
    this->setInactiveTexturePosition(0, 0);
    this->setActiveTexturePosition(8, 0);
}

bool GuiButtonElement::onMousePress(MousePressEvent &event) {
    GuiElement::onMousePress(event);
    this->texX = this->activeTexX;
    this->texY = this->activeTexY;
    this->textureUpdate = true;
    this->requireUpdate();
    return true;
}

void GuiButtonElement::onMouseRelease(MouseReleaseEvent &event) {
    GuiElement::onMouseRelease(event);
    this->texX = this->inactiveTexX;
    this->texY = this->inactiveTexY;
    this->textureUpdate = true;
    this->requireUpdate();
}