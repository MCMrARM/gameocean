#include "GuiImageElement.h"

#include "../render/RenderObjectBuilder.h"

GuiImageElement::GuiImageElement(int x, int y, Texture *texture, int texX, int texY, int w, int h) : GuiElement(x, y, w, h) {
    vertexCount = 6;
    this->setTexture(texture, texX, texY, w, h);
}

void GuiImageElement::setTexture(Texture *texture, int x, int y, int w, int h) {
    if(texture == null) {
        this->texture = GuiElement::texture;
    } else {
        this->texture = texture;
    }
    texX = x;
    texY = y;
    texW = w;
    texH = h;
    this->textureUpdate = true;
    this->requireUpdate();
}

void GuiImageElement::updateTextureId() {
    this->textureIdUpdate = true;
    this->requireUpdate();
}

void GuiImageElement::rebuild(RenderObjectBuilder *builder) {
    this->shouldRebuild = false;
    builder->rect2d(x, y, x + width, y + height, texX, texY, texX + texW, texY + texH, this->texture, r, g, b, a);
}

GuiUpdateFlags GuiImageElement::update(RenderObjectBuilder *builder) {
    GuiUpdateFlags flags;
    
    if(!this->shouldUpdate) return flags;
    this->shouldUpdate = false;

    this->rebuild(builder);
    if(this->posUpdate) {
        flags.updateVertex = true;
        this->posUpdate = false;
    }
    if(this->textureUpdate) {
        flags.updateTextureUV = true;
        this->textureUpdate = false;
    }
    if(this->textureIdUpdate) {
        flags.updateTextureId = true;
        this->textureIdUpdate = false;
    }
    return flags;
}