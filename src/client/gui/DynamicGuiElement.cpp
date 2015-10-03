#include "DynamicGuiElement.h"

#include "../render/RenderObjectBuilder.h"

void DynamicGuiElement::rebuildDynamic() {
    shouldRebuild = false;

    int vertexCount = this->getVertexCount();
    if(vertexCount == -1) {
        RenderObjectBuilder builder;
        rebuild(builder);
        renderObject = std::move(builder.getRenderObject());
    } else {
        RenderObjectBuilder builder (vertexCount);
        rebuild(builder);
        renderObject = std::move(builder.getRenderObject());
    }
    renderObjectBuilder = std::unique_ptr<RenderObjectBuilder>(new RenderObjectBuilder(*renderObject));
}

void DynamicGuiElement::updateDynamic() {
    renderObjectBuilder->pos = 0;
    GuiUpdateFlags flags = update(*renderObjectBuilder);
    this->renderObject->updateFragment(0, getVertexCount(), flags.updateVertex, flags.updateTextureUV,
                                       flags.updateTextureId, flags.updateColor);
}

void DynamicGuiElement::render() {
    if(!this->renderObject || this->hasVertexCountUpdate()) {
        this->rebuildDynamic();
    } else if(this->needsUpdate()) {
        this->updateDynamic();
    }

    if(this->renderObject) {
        this->renderObject->render();
    }
}