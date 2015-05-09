#pragma once

#include "../common.h"
#include "Texture.h"
#include "RenderObject.h"

class RenderObjectBuilder {

protected:
    bool buildTexUV = true;
    bool buildTexId = true;
    bool buildColor = true;

    virtual void addVertex(float x, float y, float z, float texU, float texV, int texId, float r, float g, float b, float a) = 0;
public:
    RenderObjectBuilder() {};
    RenderObjectBuilder(bool texUV, bool texId, bool color) : buildTexUV(texUV), buildTexId(texId), buildColor(color) {};

    inline void vertex(float x, float y, float z, float texU, float texV, int texId, float r, float g, float b, float a) { addVertex(x, y, z, texU, texV, texId, r, g, b, a); };
    inline void vertex(float x, float y, float z, float texU, float texV, Texture* tex, float r, float g, float b, float a) { addVertex(x, y, z, texU, texV, tex->getBindTextureId(), r, g, b, a); };
    inline void vertex(float x, float y, float z, float texU, float texV, int texId) { addVertex(x, y, z, texU, texV, texId, 1, 1, 1, 1); };
    inline void vertex(float x, float y, float z, float texU, float texV, Texture* tex) { addVertex(x, y, z, texU, texV, tex->getBindTextureId(), 1, 1, 1, 1); };
    inline void vertex(float x, float y, float z, float texU, float texV) { addVertex(x, y, z, texU, texV, 0, 1, 1, 1, 1); };
    inline void vertex(float x, float y, float z, float texU, float texV, float r, float g, float b, float a) { addVertex(x, y, z, texU, texV, 0, r, g, b, a); };
    inline void vertex(float x, float y, float z, float r, float g, float b, float a) { addVertex(x, y, z, 0, 0, 0, r, g, b, a); };

    void rect2d(float x, float y, float x2, float y2, float texU1, float texV1, float texU2, float texV2, int texId, float r, float g, float b, float a);
    void rect2d(float x, float y, float x2, float y2, float texU1, float texV1, float texU2, float texV2, int texId) { rect2d(x, y, x2, y2, texU1, texV1, texU2, texV2, texId, 1, 1, 1, 1); };
    //void rect2d(float x, float y, float x2, float y2, float texU1, float texV1, float texU2, float texV2) { rect2d(x, y, x2, y2, texU1, texV1, texU2, texV2, 0, 1, 1, 1, 1); };
    void rect2d(float x, float y, float x2, float y2, float r, float g, float b, float a) { rect2d(x, y, x2, y2, 0, 0, 0, 0, 0, r, g, b, a); };

    virtual RenderObject getRenderObject() = 0;

};

class StaticRenderObjectBuilder : public RenderObjectBuilder {

protected:
    const int size;
    int pos = 0;
    float* vertexArr;
    float* textureUVArr = null;
    float* textureIdArr = null;
    float* colorArr = null;

    virtual void addVertex(float x, float y, float z, float texU, float texV, int texId, float r, float g, float b, float a);

public:

    StaticRenderObjectBuilder(int size);
    StaticRenderObjectBuilder(int size, bool texUV, bool texId, bool color);

    virtual RenderObject getRenderObject();

};