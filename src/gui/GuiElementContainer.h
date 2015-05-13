#pragma once

#include <vector>
#include "DynamicGuiElement.h"

class GuiElementContainer : public DynamicGuiElement {

public:
    std::vector<GuiElement*> children;

    GuiElementContainer() : DynamicGuiElement() {};

    inline void addElement(GuiElement* element) { children.push_back(element); };

    virtual bool needsUpdate();
    virtual bool hasVertexCountUpdate();
    virtual int getVertexCount();
    virtual void rebuild(RenderObjectBuilder* builder);
    virtual GuiUpdateFlags update(RenderObjectBuilder* builder) {
        GuiUpdateFlags flags;
        return flags;
    };
    virtual void updateDynamic();

};


