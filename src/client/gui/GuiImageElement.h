#pragma once

#include "GuiElement.h"

class Texture;

class GuiImageElement : public GuiElement {

protected:

    Texture* texture;
    int texX, texY, texW, texH;

    bool textureUpdate = false;
    bool textureIdUpdate = false;

public:

    GuiImageElement(float x, float y, Texture* texture, int texX, int texY, int w, int h);

    void setTexture(Texture* texture, int x, int y, int w, int h);

    void updateTextureId();

    virtual void rebuild(RenderObjectBuilder& builder);
    virtual GuiUpdateFlags update(RenderObjectBuilder& builder);

};


