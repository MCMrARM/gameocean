#include "RenderObjectBuilder.h"

#include <algorithm>
#include <cstring>

RenderObjectBuilder::RenderObjectBuilder(int size, bool texUV, bool texId, bool color) {
    this->size = size;
    vertexArr = std::shared_ptr<std::vector<float>>(new std::vector<float>(size * 3));
    if (texUV)
        textureUVArr = std::shared_ptr<std::vector<float>>(new std::vector<float>(size * 2));
    if (texId)
        textureIdArr = std::shared_ptr<std::vector<float>>(new std::vector<float>(size));
    if (color)
        colorArr = std::shared_ptr<std::vector<float>>(new std::vector<float>(size * 4));
}

RenderObjectBuilder::RenderObjectBuilder(std::shared_ptr<std::vector<float>> vertex, std::shared_ptr<std::vector<float>> texUV,
                                         std::shared_ptr<std::vector<float>> texId, std::shared_ptr<std::vector<float>> color) {
    this->size = vertex->size() / 3;
    this->vertexArr = vertex;
    this->textureUVArr = texUV;
    this->textureIdArr = texId;
    this->colorArr = color;
}

RenderObjectBuilder::RenderObjectBuilder(RenderObject& object) : RenderObjectBuilder(object.vertexArray,
                                                                                     object.textureUVArray,
                                                                                     object.textureIdArray,
                                                                                     object.colorArray) {
}

void RenderObjectBuilder::rect2d(float x, float y, float x2, float y2, float texU1, float texV1, float texU2,
                                 float texV2, int texId, Color color) {
    vertex(x, y, 0.0f, texU1, texV1, texId, color);
    vertex(x2, y, 0.0f, texU2, texV1, texId, color);
    vertex(x, y2, 0.0f, texU1, texV2, texId, color);

    vertex(x2, y, 0.0f, texU2, texV1, texId, color);
    vertex(x2, y2, 0.0f, texU2, texV2, texId, color);
    vertex(x, y2, 0.0f, texU1, texV2, texId, color);
}

void RenderObjectBuilder::addVertex(float x, float y, float z,
                                    float texU, float texV, int texId,
                                    float r, float g, float b, float a) {
    if (renderObjectBuilt) {
        vertexArr = std::shared_ptr<std::vector<float>>(new std::vector<float>(*vertexArr));
        textureUVArr = std::shared_ptr<std::vector<float>>(new std::vector<float>(*textureUVArr));
        textureIdArr = std::shared_ptr<std::vector<float>>(new std::vector<float>(*textureIdArr));
        colorArr = std::shared_ptr<std::vector<float>>(new std::vector<float>(*colorArr));
        renderObjectBuilt = false;
    }
    if (pos >= size) {
        if (size <= 0)
            size = 16;
        Logger::main->trace("StaticRenderObjectBuilder", "Reallocating array; new size: %i; from: %i", size * 2, size);
        size *= 2;
        if (!vertexArr->empty())
            vertexArr->resize(size * 3);
        if (!textureUVArr->empty())
            textureUVArr->resize(size * 2);
        if (!textureIdArr->empty())
            textureIdArr->resize(size);
        if (!colorArr->empty())
            colorArr->resize(size * 4);
    }
    int vPos = pos * 3;
    (*vertexArr)[vPos] = x;
    (*vertexArr)[vPos + 1] = y;
    (*vertexArr)[vPos + 2] = z;

    if (!textureUVArr->empty()) {
        int texPos = pos * 2;
        (*textureUVArr)[texPos] = texU;
        (*textureUVArr)[texPos + 1] = texV;
    }

    if (!textureIdArr->empty()) {
        (*textureIdArr)[pos] = texId;
    }

    if (!colorArr->empty()) {
        int colorPos = pos * 4;
        (*colorArr)[colorPos] = r;
        (*colorArr)[colorPos + 1] = g;
        (*colorArr)[colorPos + 2] = b;
        (*colorArr)[colorPos + 3] = a;
    }

    pos++;
}

std::unique_ptr<RenderObject> RenderObjectBuilder::getRenderObject() {
    renderObjectBuilt = true;
    std::unique_ptr<RenderObject> obj (new RenderObject(vertexArr, textureUVArr, textureIdArr, colorArr));
    obj->update();
    return std::move(obj);
}