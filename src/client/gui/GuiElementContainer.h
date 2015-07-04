#pragma once

#include <string>
#include <vector>
#include "DynamicGuiElement.h"
#include "../input/TouchHandler.h"

class GuiElementContainer : public DynamicGuiElement {

protected:
    GuiElement* focusedElement = null;
    GuiElement* clickedElement = null;
    GuiElement** touchedElements;

public:
    std::vector<GuiElement*> children;
    bool clip = true;

    GuiElementContainer();
    ~GuiElementContainer();

    inline void addElement(GuiElement* element) { children.push_back(element); };

    virtual void render();

    virtual bool needsUpdate();
    virtual bool hasVertexCountUpdate();
    virtual int getVertexCount();
    virtual void rebuild(RenderObjectBuilder* builder);
    virtual GuiUpdateFlags update(RenderObjectBuilder* builder) {
        GuiUpdateFlags flags;
        return flags;
    };
    virtual void updateDynamic();

    inline GuiElement* getFocusedElement() { return focusedElement; };

    virtual bool supportsMultitouch() { return true; };
    virtual bool onMousePress(MousePressEvent& event);
    virtual void onMouseMove(MouseMoveEvent& event);
    virtual void onMouseRelease(MouseReleaseEvent& event);

    virtual void setText(std::string newText);

};


