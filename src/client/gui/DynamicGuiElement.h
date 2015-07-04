#pragma once

#include "common.h"
#include "GuiElement.h"
class RenderObject;
class RenderObjectBuilder;

class DynamicGuiElement : public GuiElement {

protected:
    RenderObject* renderObject = null;
    RenderObjectBuilder* renderObjectBuilder = null;

public:
    DynamicGuiElement() : GuiElement() {};
    ~DynamicGuiElement();

    virtual bool isDynamic() { return true; };

    virtual void render();

    virtual void rebuildDynamic();
    virtual void updateDynamic();

};


