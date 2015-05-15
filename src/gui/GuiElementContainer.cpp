#include "GuiElementContainer.h"
#include "opengl.h"
#include "../render/RenderObjectBuilder.h"
#include "../input/TouchHandler.h"
#include "../input/TouchEvent.h"
#include "../App.h"

GuiElementContainer::GuiElementContainer() : DynamicGuiElement() {
    touchedElements = new GuiElement*[TouchHandler::MAX_TOUCHES];
    for (int i = 0; i < TouchHandler::MAX_TOUCHES; i++) {
        touchedElements[i] = null;
    }
}

GuiElementContainer::~GuiElementContainer() {
    delete[] touchedElements;
}

bool GuiElementContainer::needsUpdate() {
    for (GuiElement *el : children) {
        if (el->needsUpdate()) return true;
    }
    return false;
}

bool GuiElementContainer::hasVertexCountUpdate() {
    for (GuiElement *el : children) {
        if (el->hasVertexCountUpdate()) return true;
    }
    return false;
}

int GuiElementContainer::getVertexCount() {
    int count = 0;
    for (GuiElement *el : children) {
        int c = el->getVertexCount();
        if (c == -1) { return -1; }
        count += c;
    }
    return count;
}

void GuiElementContainer::rebuild(RenderObjectBuilder *builder) {
    for (GuiElement *el : children) {
        el->builderOffset = builder->pos;
        el->rebuild(builder);
    }
}

void GuiElementContainer::updateDynamic() {
    RenderObjectBuilder *builder = this->renderObjectBuilder;
    for (GuiElement *el : children) {
        builder->pos = el->builderOffset;
        GuiUpdateFlags flags = el->update(builder);
        this->renderObject->updateFragment(el->builderOffset, el->getVertexCount(), flags.updateVertex,
                                           flags.updateTextureUV, flags.updateTextureId, flags.updateColor);
    }
}

bool GuiElementContainer::onMousePress(MousePressEvent& event) {
    bool touch = event.isTouch();
    if (touch) {
        int fId = ((TouchPressEvent&) event).fingerId;
        if (fId >= TouchHandler::MAX_TOUCHES) { return true; }
    }

    for (auto it = children.rbegin(); it != children.rend(); it++) {
        GuiElement* el = *it;

        if (el->isPointInside(event.x, event.y)) {
            if (touch && !el->supportsMultitouch()) {
                for (int i = 0; i < TouchHandler::MAX_TOUCHES; i++) {
                    if (el == touchedElements[i]) {
                        return true;
                    }
                }
            }
            if (!el->onMousePress(event)) continue;
            if (touch) {
                int fId = ((TouchPressEvent&) event).fingerId;
                touchedElements[fId] = el;
            } else {
                clickedElement = el;
            }
            return true;
        }
    }
    return true;
}

void GuiElementContainer::onMouseMove(MouseMoveEvent& event) {
    if (event.isTouch()) {
        TouchMoveEvent& touchEvent = (TouchMoveEvent&) event;
        int fId = touchEvent.fingerId;
        if (fId >= TouchHandler::MAX_TOUCHES) { return; }

        if (touchedElements[fId] != null) {
            touchedElements[fId]->onMouseMove(event);
        }
    } else {
        if (clickedElement != null) {
            clickedElement->onMouseMove(event);
        }
    }
}

void GuiElementContainer::onMouseRelease(MouseReleaseEvent& event) {
    if (event.isTouch()) {
        TouchReleaseEvent& touchEvent = (TouchReleaseEvent&) event;
        int fId = touchEvent.fingerId;
        if (fId >= TouchHandler::MAX_TOUCHES) { return; }

        if (touchedElements[fId] != null) {
            touchedElements[fId]->onMouseRelease(event);
            touchedElements[fId] = null;
        }
    } else {
        if (clickedElement != null) {
            clickedElement->onMouseRelease(event);
            clickedElement = null;
        }
    }
}

void GuiElementContainer::render() {
    if(clip) {
        int pxSize = App::instance->pixelSize;
        glViewport(this->x * pxSize, this->y * pxSize, (this->x + this->width) * pxSize,
                   (this->y + this->height) * pxSize);
    }
    DynamicGuiElement::render();
    if(clip) {
        glViewport(0, 0, App::instance->screenWidth, App::instance->screenHeight);
    }
}