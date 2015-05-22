#include "GuiButtonElement.h"

#include "../common.h"
#include "../render/Font.h"
#include "../render/RenderObjectBuilder.h"

GuiButtonElement::GuiButtonElement(int x, int y, int w, int h, std::string text) : GuiNinePathImageElement(x, y, w, h, null, 0, 0, 8, 8, 3) {
    this->setInactiveTexturePosition(0, 0);
    this->setActiveTexturePosition(8, 0);
    inactiveColor = {1.f, 1.f, 1.f, 1.f};
    activeColor = {1.f, 1.f, 0.5f, 1.f};
    textColor = inactiveColor;
    this->setText(text);
}

bool GuiButtonElement::onMousePress(MousePressEvent &event) {
    GuiElement::onMousePress(event);
    this->texX = this->activeTexX;
    this->texY = this->activeTexY;
    textColor = activeColor;
    this->textureUpdate = true;
    this->colorUpdate = true;
    this->requireUpdate();
    return true;
}

void GuiButtonElement::onMouseRelease(MouseReleaseEvent &event) {
    GuiElement::onMouseRelease(event);
    this->texX = this->inactiveTexX;
    this->texY = this->inactiveTexY;
    textColor = inactiveColor;
    this->textureUpdate = true;
    this->colorUpdate = true;
    this->requireUpdate();
}

void GuiButtonElement::rebuild(RenderObjectBuilder *builder) {
    GuiNinePathImageElement::rebuild(builder);
    Font::main->build(builder, x + width / 2 - textW / 2 + 1, y + height / 2 - 6, text, {0.4f, 0.4f, 0.4f, 1.f});
    Font::main->build(builder, x + width / 2 - textW / 2, y + height / 2 - 5, text, textColor);
}

void GuiButtonElement::setText(std::string newText) {
    this->text = newText;
    textW = Font::main->getWidth(newText);
    vertexCount = 9 * 6 + 2 * Font::main->getVertexCount(newText);
    this->requireRebuild();
}