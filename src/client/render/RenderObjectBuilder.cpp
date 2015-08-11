#include "RenderObjectBuilder.h"

#include <algorithm>
#include <cstring>

RenderObjectBuilder::RenderObjectBuilder(int size, bool texUV, bool texId, bool color) : buildTexUV(texUV),
                                                                                         buildTexId(texId),
                                                                                         buildColor(color) {
    this->size = size;
    this->reallocate();
}

RenderObjectBuilder::RenderObjectBuilder(int size, float *vertex, float *texUV, float *texId, float *color) {
    this->size = size;
    this->vertexArr = vertex;
    this->textureUVArr = texUV;
    this->buildTexUV = (texUV != null);
    this->textureIdArr = texId;
    this->buildTexId = (texId != null);
    this->colorArr = color;
    this->buildColor = (color != null);
}

RenderObjectBuilder::RenderObjectBuilder(RenderObject& object) : RenderObjectBuilder(object.size, object.vertexArray,
                                                                                     object.textureUVArray,
                                                                                     object.textureIdArray,
                                                                                     object.colorArray) {
    //
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
    int vPos = pos * 3;
    vertexArr[vPos] = x;
    vertexArr[vPos + 1] = y;
    vertexArr[vPos + 2] = z;

    if (this->buildTexUV) {
        int texPos = pos * 2;
        textureUVArr[texPos] = texU;
        textureUVArr[texPos + 1] = texV;
    }

    if (this->buildTexId) {
        textureIdArr[pos] = texId;
    }

    if (this->buildColor) {
        int colorPos = pos * 4;
        colorArr[colorPos] = r;
        colorArr[colorPos + 1] = g;
        colorArr[colorPos + 2] = b;
        colorArr[colorPos + 3] = a;
    }

    pos++;
}

void RenderObjectBuilder::freeArrays() {
    if (this->vertexArr != null) {
        delete this->vertexArr;
        this->vertexArr = null;
    }
    if (this->textureUVArr != null) {
        delete this->textureUVArr;
        this->textureUVArr = null;
    }
    if (this->textureIdArr != null) {
        delete this->textureIdArr;
        this->textureIdArr = null;
    }
    if (this->colorArr != null) {
        delete this->colorArr;
        this->colorArr = null;
    }
    this->pos = -1;
}

void RenderObjectBuilder::reallocateArray(float *&arr, int entrySize, int newSize, bool deleteOld) {
    if (arr != null) {
        float *newVertex = new float[newSize * entrySize];
        memcpy(newVertex, arr, std::min(size, newSize) * entrySize * sizeof(float));
        if (deleteOld) { delete arr; }
        arr = newVertex;
    } else {
        arr = new float[newSize * entrySize];
    }
}

void RenderObjectBuilder::reallocate(int newSize, bool deleteOld) {
    this->reallocateArray(this->vertexArr, 3, newSize, deleteOld);
    if (this->buildTexUV) {
        this->reallocateArray(this->textureUVArr, 2, newSize, deleteOld);
    }
    if (this->buildTexId) {
        this->reallocateArray(this->textureIdArr, 1, newSize, deleteOld);
    }
    if (this->buildColor) {
        this->reallocateArray(this->colorArr, 4, newSize, deleteOld);
    }
}

RenderObject* RenderObjectBuilder::getRenderObject() {
    RenderObject* obj = new RenderObject(size, vertexArr, textureUVArr, textureIdArr, colorArr);
    obj->update();
    return obj;
}

void StaticRenderObjectBuilder::addVertex(float x, float y, float z,
                                          float texU, float texV, int texId,
                                          float r, float g, float b, float a) {
    if (pos >= size) {
        Logger::main->warn("StaticRenderObjectBuilder", "Trying to add a vertex out of the buffer (pos: %i, size: %i)",
                           pos, size);
        return;
    }
    RenderObjectBuilder::addVertex(x, y, z, texU, texV, texId, r, g, b, a);
}

void DynamicRenderObjectBuilder::addVertex(float x, float y, float z, float texU, float texV, int texId, float r,
                                           float g, float b, float a) {
    if (pos >= size) {
        Logger::main->trace("StaticRenderObjectBuilder", "Reallocating array; new size: %i; from: %i", size * 2, size);

        this->reallocate(size * 2, true);
    }
    RenderObjectBuilder::addVertex(x, y, z, texU, texV, texId, r, g, b, a);
}

RenderObject* DynamicRenderObjectBuilder::getRenderObject(bool realloc) {
    if (realloc) {
        float *vertex = new float[size * 3];
        memcpy(vertex, vertexArr, pos * 3);
        float *texUV = new float[size * 2];
        memcpy(texUV, textureUVArr, pos * 2);
        float *texId = new float[size];
        memcpy(texId, textureIdArr, pos);
        float *color = new float[size * 4];
        memcpy(color, colorArr, pos * 4);
        RenderObject* obj = new RenderObject(size, vertex, texUV, texId, color);
        obj->update();
        return obj;
    } else {
        RenderObject* obj = new RenderObject(size, vertexArr, textureUVArr, textureIdArr, colorArr);
        obj->update();
        return obj;
    }
}