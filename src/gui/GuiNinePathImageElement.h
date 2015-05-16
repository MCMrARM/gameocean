#pragma once

#include "GuiImageElement.h"

class GuiNinePathImageElement : public GuiImageElement {

    int texOffX1, texOffY1, texOffX2, texOffY2;

public:

    GuiNinePathImageElement(int x, int y, int w, int h, Texture* texture, int texX, int texY, int texW, int texH, int texOff);

    void setTextureOffset(int x1, int y1, int x2, int y2);

    virtual void rebuild(RenderObjectBuilder* builder);

};