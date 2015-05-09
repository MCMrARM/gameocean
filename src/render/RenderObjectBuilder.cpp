#include "RenderObjectBuilder.h"

void RenderObjectBuilder::rect2d(float x, float y, float x2, float y2, float texU1, float texV1, float texU2,
                                 float texV2, int texId, float r, float g, float b, float a) {
    vertex(x, y, 0.0f, texU1, texV1, texId, r, g, b, a);
    vertex(x2, y, 0.0f, texU1, texV1, texId, r, g, b, a);
    vertex(x, y2, 0.0f, texU1, texV1, texId, r, g, b, a);

    vertex(x2, y, 0.0f, texU2, texV1, texId, r, g, b, a);
    vertex(x2, y2, 0.0f, texU2, texV2, texId, r, g, b, a);
    vertex(x, y2, 0.0f, texU1, texV2, texId, r, g, b, a);
}

StaticRenderObjectBuilder::StaticRenderObjectBuilder(int size, bool texUV, bool texId, bool color) : RenderObjectBuilder(texUV, texId, color), size(size) {
    this->vertexArr = new float[size * 3];
    if(this->buildTexUV) { this->textureUVArr = new float[size * 2]; }
    if(this->buildTexId) { this->textureIdArr = new float[size]; }
    if(this->buildColor) { this->colorArr = new float[size * 4]; }
}

StaticRenderObjectBuilder::StaticRenderObjectBuilder(int size) : StaticRenderObjectBuilder(size, true, true, true) {
}

void StaticRenderObjectBuilder::addVertex(float x, float y, float z,
                                       float texU, float texV, int texId,
                                       float r, float g, float b, float a) {
    if(pos >= size) {
        Logger::main->warn("StaticRenderObjectBuilder", "Trying to add a vertex out of the buffer (pos: %i, size: %i)", pos, size);
        return;
    }

    int vPos = pos * 3;
    vertexArr[vPos] = x;
    vertexArr[vPos + 1] = y;
    vertexArr[vPos + 2] = z;

    if(this->buildTexUV) {
        int texPos = pos * 2;
        textureUVArr[texPos] = texU;
        textureUVArr[texPos + 1] = texV;
    }

    if(this->buildTexId) {
        textureIdArr[pos] = texId;
    }

    if(this->buildColor) {
        int colorPos = pos * 4;
        colorArr[colorPos] = r;
        colorArr[colorPos + 1] = g;
        colorArr[colorPos + 2] = b;
        colorArr[colorPos + 3] = a;
    }

    pos++;
}

RenderObject StaticRenderObjectBuilder::getRenderObject() {
    RenderObject obj (size, vertexArr, textureUVArr, textureIdArr, colorArr);
    obj.update();
    return obj;
}