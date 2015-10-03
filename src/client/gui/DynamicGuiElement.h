#pragma once

#include <memory>
#include "common.h"
#include "GuiElement.h"
#include "../render/RenderObject.h"
#include "../render/RenderObjectBuilder.h"
#include "GuiElement.h"

class DynamicGuiElement : public GuiElement {

protected:
    std::unique_ptr<RenderObject> renderObject = null;
    std::unique_ptr<RenderObjectBuilder> renderObjectBuilder = null;

public:
    DynamicGuiElement() : GuiElement() {};

    virtual bool isDynamic() { return true; };

    virtual void render();

    virtual void rebuildDynamic();
    virtual void updateDynamic();

};


