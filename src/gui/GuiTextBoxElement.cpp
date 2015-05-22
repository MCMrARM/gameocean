#include "GuiTextBoxElement.h"

#include "../App.h"
#include "../render/Font.h"
#include "../render/RenderObjectBuilder.h"
#include "../utils/Time.h"

GuiTextBoxElement::GuiTextBoxElement(int x, int y, int w, int h, std::string text) : GuiNinePathImageElement(x, y, w, h, null, 0, 16, 8, 8, 3) {
    this->setInactiveTexturePosition(0, 16);
    this->setActiveTexturePosition(8, 16);
    textColor = {1.f, 1.f, 1.f, 1.f};
    this->setText(text);
    carretVisible = false;
}

void GuiTextBoxElement::focus() {
    this->active = true;

    this->texX = this->activeTexX;
    this->texY = this->activeTexY;
    this->textureUpdate = true;
    this->carretVisible = false;
    this->colorUpdate = true;
    this->requireUpdate();

    App::instance->showKeyboard(this->text);
}

void GuiTextBoxElement::blur() {
    this->active = false;

    this->texX = this->inactiveTexX;
    this->texY = this->inactiveTexY;
    this->textureUpdate = true;
    this->carretVisible = false;
    this->colorUpdate = true;
    this->requireUpdate();

    App::instance->hideKeyboard();
}

void GuiTextBoxElement::buildCarret(RenderObjectBuilder *builder) {
    Color c = textColor;
    if (!carretVisible) {
        c.a = 0.f;
    }
    builder->rect2d(x + 3 + textW, y + height / 2 - 5, x + 3 + textW + 1, y + height / 2 + 5, 0, 0, 1, 1, Texture::EMPTY, c);
}

void GuiTextBoxElement::rebuild(RenderObjectBuilder *builder) {
    GuiNinePathImageElement::rebuild(builder);
    Font::main->build(builder, x + 3, y + height / 2 - 6, text, {0.4f, 0.4f, 0.4f, 1.f});
    Font::main->build(builder, x + 2, y + height / 2 - 5, text, textColor);

    buildCarret(builder);
}

GuiUpdateFlags GuiTextBoxElement::update(RenderObjectBuilder *builder) {
    if (this->shouldUpdate) {
        return GuiImageElement::update(builder);
    }
    
    GuiUpdateFlags flags;

    if (!active || vertexCount < 6) { return flags; }

    builder->pos = builderOffset + vertexCount - 6;

    // update the carret
    long long now = Time::now();

    if (carretTime + CARRET_BLINK_TIME <= now) {
        carretVisible = !carretVisible;

        buildCarret(builder);
        flags.updateColor = true;

        carretTime = now;
    }
    
    return flags;
}

void GuiTextBoxElement::setText(std::string newText) {
    this->text = newText;
    textW = Font::main->getWidth(newText);
    vertexCount = 9 * 6 + 2 * Font::main->getVertexCount(newText) + 6;
    this->requireRebuild();

    carretVisible = true;
    carretTime = Time::now();
}