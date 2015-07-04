#include "GuiNinePathImageElement.h"

#include "../render/RenderObjectBuilder.h"

GuiNinePathImageElement::GuiNinePathImageElement(int x, int y, int w, int h, Texture *texture, int texX, int texY,
                                                 int texW, int texH, int texOff) : GuiImageElement(x, y, texture, texX, texY, texW, texH) {
    this->width = w;
    this->height = h;
    this->texOffX1 = texOff;
    this->texOffY1 = texOff;
    this->texOffX2 = texOff;
    this->texOffY2 = texOff;
    vertexCount = 9 * 6;
}

void GuiNinePathImageElement::setTextureOffset(int x1, int y1, int x2, int y2) {
    this->texOffX1 = x1;
    this->texOffY1 = y1;
    this->texOffX2 = x2;
    this->texOffY2 = y2;
    this->textureUpdate = true;
    this->requireUpdate();
}

void GuiNinePathImageElement::rebuild(RenderObjectBuilder *builder) {
    this->shouldRebuild = false;
    //builder->rect2d(x, y, x + width, y + height, texX, texY, texX + texW, texY + texH, this->texture, r, g, b, a);
    builder->rect2d(x, y + height - texOffY2, x + texOffX1, y + height, texX, texY, texX + texOffX1, texY + texOffY2, this->texture, color);
    builder->rect2d(x, y + texOffY1, x + texOffX1, y + height - texOffY2, texX, texY + texOffY2, texX + texOffX1, texY + texH - texOffY1, this->texture, color);
    builder->rect2d(x, y, x + texOffX1, y + texOffY1, texX, texY + texH - texOffY1, texX + texOffX1, texY + texH, this->texture, color);

    builder->rect2d(x + texOffX1, y + height - texOffY2, x + width - texOffX2, y + height, texX + texOffX1, texY, texX + texW - texOffX2, texY + texOffY2, this->texture, color);
    builder->rect2d(x + texOffX1, y + texOffY1, x + width - texOffX2, y + height - texOffY2, texX + texOffX1, texY + texOffY2, texX + texW - texOffX2, texY + texH - texOffY1, this->texture, color);
    builder->rect2d(x + texOffX1, y, x + width - texOffX2, y + texOffY1, texX + texOffX1, texY + texH - texOffY1, texX + texW - texOffX2, texY + texH, this->texture, color);

    builder->rect2d(x + width - texOffX2, y + height - texOffY2, x + width, y + height, texX + texW - texOffX2, texY, texX + texW, texY + texOffY2, this->texture, color);
    builder->rect2d(x + width - texOffX2, y + texOffY1, x + width, y + height - texOffY2, texX + texW - texOffX2, texY + texOffY2, texX + texW, texY + texH - texOffY1, this->texture, color);
    builder->rect2d(x + width - texOffX2, y, x + width, y + texOffY1, texX + texW - texOffX2, texY + texH - texOffY1, texX + texW, texY + texH, this->texture, color);
}