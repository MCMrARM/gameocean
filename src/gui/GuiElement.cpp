#include "GuiElement.h"

#include "../utils/Logger.h"
#include "../render/TextureManager.h"

Texture* GuiElement::texture = null;

void GuiElement::initTexture() {
    GuiElement::texture = TextureManager::require("images/gui.png");
}

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

bool GuiElement::isPointInside(int x, int y) {
    if(x >= this->x && y >= this->y && x <= this->x + this->width && y <= this->y + this->height) {
        return true;
    }
    return false;
}

void GuiElement::onMouseRelease(MouseReleaseEvent& event) {
    if(this->isPointInside(event.x, event.y)) {
        this->click();
    }
}