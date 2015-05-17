#include "GuiButtonElement.h"

#include "../common.h"
#include "../render/Font.h"
#include "../render/RenderObjectBuilder.h"

GuiButtonElement::GuiButtonElement(int x, int y, int w, int h, std::string text) : GuiNinePathImageElement(x, y, w, h, null, 0, 0, 8, 8, 3) {
    this->setInactiveTexturePosition(0, 0);
    this->setActiveTexturePosition(8, 0);
    this->text = text;
    vertexCount = 9 * 6 + 2 * Font::main->getVertexCount(text);
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

void GuiButtonElement::rebuild(RenderObjectBuilder *builder) {
    GuiNinePathImageElement::rebuild(builder);
    Font::main->build(builder, 1, -1, text, {0.4f, 0.4f, 0.4f, 1.f});
    Font::main->build(builder, 0, 0, text, {0.f, 0.f, 0.f, 1.f});
}