#include "DynamicGuiElement.h"

#include "../render/RenderObjectBuilder.h"

void DynamicGuiElement::rebuildDynamic() {
    shouldRebuild = false;

    if(this->renderObject != null) {
        delete this->renderObject;
    }
    if(this->renderObjectBuilder != null) {
        delete this->renderObjectBuilder;
    }

    int vertexCount = this->getVertexCount();
    if(vertexCount == -1) {
        DynamicRenderObjectBuilder builder;
        rebuild(&builder);
        this->renderObject = builder.getRenderObject();
        this->renderObjectBuilder = new StaticRenderObjectBuilder(*renderObject);
    } else {
        StaticRenderObjectBuilder* builder = new StaticRenderObjectBuilder(vertexCount);
        rebuild(builder);
        this->renderObject = builder->getRenderObject();
        this->renderObjectBuilder = builder;
    }
}

DynamicGuiElement::~DynamicGuiElement() {
    if(this->renderObject != null) {
        delete this->renderObject;
    }
    if(this->renderObjectBuilder != null) {
        delete this->renderObjectBuilder;
    }
}

void DynamicGuiElement::updateDynamic() {
    renderObjectBuilder->pos = 0;
    GuiUpdateFlags flags = update(renderObjectBuilder);
    this->renderObject->updateFragment(0, getVertexCount(), flags.updateVertex, flags.updateTextureUV,
                                       flags.updateTextureId, flags.updateColor);
}

void DynamicGuiElement::render() {
    if(this->renderObject == null || this->hasVertexCountUpdate()) {
        this->rebuildDynamic();
    } else if(this->needsUpdate()) {
        this->updateDynamic();
    }

    if(this->renderObject != null) {
        this->renderObject->render();
    }
}