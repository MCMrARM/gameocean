#include "GuiElementContainer.h"
#include "opengl.h"
#include "../render/Shader.h"
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
    for (std::shared_ptr<GuiElement> const& el : children) {
        if (el->needsUpdate()) return true;
    }
    return false;
}

bool GuiElementContainer::hasVertexCountUpdate() {
    if (shouldRebuild)
        return true;
    for (std::shared_ptr<GuiElement> const& el : children) {
        if (el->hasVertexCountUpdate()) return true;
    }
    return false;
}

int GuiElementContainer::getVertexCount() {
    int count = 0;
    for (std::shared_ptr<GuiElement> const& el : children) {
        int c = el->getVertexCount();
        if (c == -1) { return -1; }
        count += c;
    }
    return count;
}

void GuiElementContainer::rebuild(RenderObjectBuilder& builder) {
    if (shouldRebuild) {
        shouldRebuild = false;
    }
    for (std::shared_ptr<GuiElement> const& el : children) {
        el->builderOffset = builder.pos;
        el->rebuild(builder);
    }
}

void GuiElementContainer::updateDynamic() {
    RenderObjectBuilder& builder = *renderObjectBuilder;
    for (std::shared_ptr<GuiElement> const& el : children) {
        builder.pos = el->builderOffset;
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
        std::shared_ptr<GuiElement> el = *it;

        if (el->isPointInside(event.x, event.y)) {
            if (touch && !el->supportsMultitouch()) {
                for (int i = 0; i < TouchHandler::MAX_TOUCHES; i++) {
                    if (&*el == touchedElements[i]) {
                        return true;
                    }
                }
            }
            if (!el->onMousePress(event)) continue;
            if (focusedElement != null) { focusedElement->blur(); }
            focusedElement = &*el;
            el->focus();
            if (touch) {
                int fId = ((TouchPressEvent&) event).fingerId;
                touchedElements[fId] = &*el;
            } else {
                clickedElement = &*el;
            }
            return true;
        }
    }
    if (focusedElement != null) { focusedElement->blur(); }
    focusedElement = null;
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

void GuiElementContainer::setText(std::string text) {
    if (this->focusedElement != null) {
        this->focusedElement->setText(text);
    }
}

void GuiElementContainer::render() {
    if(Shader::current == null) { Logger::main->warn("Shader", "Tried to render a GuiElementContainer without a shader set!"); return; }
    if(clip) {
        int pxSize = App::instance->pixelSize;
        glViewport(this->x * pxSize, this->y * pxSize, (this->x + this->width) * pxSize,
                   (this->y + this->height) * pxSize);
    }
    int cu = Shader::current->colorUniform();
    if (cu != -1)
        glUniform4f(cu, color.r, color.g, color.b, color.a);
    DynamicGuiElement::render();
    if (cu != -1)
        glUniform4f(cu, 1.f, 1.f, 1.f, 1.f);
    if(clip) {
        glViewport(0, 0, App::instance->screenWidth, App::instance->screenHeight);
    }
}