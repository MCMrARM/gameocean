#include "GuiElementContainer.h"
#include "../render/RenderObjectBuilder.h"

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