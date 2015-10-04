#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include "DynamicGuiElement.h"
#include "../input/TouchHandler.h"

class GuiElementContainer : public DynamicGuiElement {

protected:
    GuiElement* focusedElement = null;
    GuiElement* clickedElement = null;
    GuiElement** touchedElements;

public:
    std::vector<std::shared_ptr<GuiElement>> children;
    bool clip = true;

    GuiElementContainer();
    ~GuiElementContainer();

    inline void addElement(std::shared_ptr<GuiElement> element) {
        element->requireRebuild();
        children.push_back(element);
        //requireRebuild();
    };
    void removeElement(std::shared_ptr<GuiElement> element) {
        children.erase(std::remove(children.begin(), children.end(), element), children.end());
        requireRebuild();
    };

    virtual void render();

    virtual bool needsUpdate();
    virtual bool hasVertexCountUpdate();
    virtual int getVertexCount();
    virtual void rebuild(RenderObjectBuilder& builder);
    virtual GuiUpdateFlags update(RenderObjectBuilder& builder) {
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


