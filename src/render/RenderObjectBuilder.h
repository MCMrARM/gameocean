#pragma once

#include "../common.h"
#include "Texture.h"
#include "RenderObject.h"

class RenderObjectBuilder {

protected:
    bool buildTexUV = true;
    bool buildTexId = true;
    bool buildColor = true;

    int size;
    float* vertexArr = null;
    float* textureUVArr = null;
    float* textureIdArr = null;
    float* colorArr = null;

    virtual void addVertex(float x, float y, float z, float texU, float texV, int texId, float r, float g, float b, float a);

    virtual void reallocateArray(float*& arr, int entrySize, int newSize, bool deleteOld);

public:
    int pos = 0;

    RenderObjectBuilder(int size, bool texUV, bool texId, bool color);
    RenderObjectBuilder(int size) : RenderObjectBuilder(size, true, true, true) {};
    RenderObjectBuilder(int size, float* vertex, float* texUV, float* texId, float* color);
    RenderObjectBuilder(RenderObject& object);

    inline void vertex(float x, float y, float z, float texU, float texV, int texId, float r, float g, float b, float a) {
        addVertex(x, y, z, texU, texV, texId, r, g, b, a);
    };
    inline void vertex(float x, float y, float z, int texX, int texY, Texture* tex, float r, float g, float b, float a) {
        addVertex(x, y, z, (float) texX / tex->getWidth(), (float) (tex->getHeight() - texY) / tex->getHeight(), tex->getBindTextureId(), r, g, b, a);
    };
    inline void vertex(float x, float y, float z, float texU, float texV, int texId) {
        addVertex(x, y, z, texU, texV, texId, 1, 1, 1, 1);
    };
    inline void vertex(float x, float y, float z, int texX, int texY, Texture* tex) {
        addVertex(x, y, z, texX, texY, tex->getBindTextureId(), 1, 1, 1, 1);
    };
    inline void vertex(float x, float y, float z, float texU, float texV) {
        addVertex(x, y, z, texU, texV, 0, 1, 1, 1, 1);
    };
    inline void vertex(float x, float y, float z, float texU, float texV, float r, float g, float b, float a) {
        addVertex(x, y, z, texU, texV, 0, r, g, b, a);
    };
    inline void vertex(float x, float y, float z, float r, float g, float b, float a) {
        addVertex(x, y, z, 0, 0, 0, r, g, b, a);
    };

    void rect2d(float x, float y, float x2, float y2, float texU1, float texV1, float texU2, float texV2, int texId, float r, float g, float b, float a);
    inline void rect2d(float x, float y, float x2, float y2, float texU1, float texV1, float texU2, float texV2, int texId) {
        rect2d(x, y, x2, y2, texU1, texV1, texU2, texV2, texId, 1, 1, 1, 1);
    };
    inline void rect2d(float x, float y, float x2, float y2, int texX1, int texY1, int texX2, int texY2, Texture* tex, float r, float g, float b, float a) {
        rect2d(x, y, x2, y2, (float) texX1 / tex->getWidth(), (float) (tex->getHeight() - texY2) / tex->getHeight(),
               (float) texX2 / tex->getWidth(), (float) (tex->getHeight() - texY1) / tex->getHeight(),
               tex->getBindTextureId(), r, g, b, a);
    };
    inline void rect2d(float x, float y, float x2, float y2, int texX1, int texY1, int texX2, int texY2, Texture* tex) {
        rect2d(x, y, x2, y2, texX1, texY1, texX2, texY2, tex, 1, 1, 1, 1);
    };
    inline void rect2d(float x, float y, float x2, float y2, float r, float g, float b, float a) {
        rect2d(x, y, x2, y2, 0.f, 0.f, 0.f, 0.f, 0, r, g, b, a);
    };

    virtual RenderObject* getRenderObject();

    virtual void freeArrays();
    /**
     * Use this function to either reuse this object after calling freeArrays() or if you want to create a second
     * RenderObject from it
     */
    virtual void reallocate(int newSize, bool deleteOld);
    void reallocate() { reallocate(size, false); };

};

class StaticRenderObjectBuilder : public RenderObjectBuilder {

protected:
    virtual void addVertex(float x, float y, float z, float texU, float texV, int texId, float r, float g, float b, float a);

public:
    StaticRenderObjectBuilder(int size, bool texUV, bool texId, bool color) : RenderObjectBuilder(size, texUV, texId, color) {};
    StaticRenderObjectBuilder(int size) : StaticRenderObjectBuilder(size, true, true, true) {};
    //StaticRenderObjectBuilder(int size, float* vertex, float* texUV, float* texId, float* color) : StaticRenderObjectBuilder(size, vertex, texUV, texId, color) {};
    StaticRenderObjectBuilder(RenderObject& object) : RenderObjectBuilder(object) {};

};

class DynamicRenderObjectBuilder : public RenderObjectBuilder {

protected:
    static const int DEFAULT_SIZE = 512;

    virtual void addVertex(float x, float y, float z, float texU, float texV, int texId, float r, float g, float b, float a);

public:

    DynamicRenderObjectBuilder(int size, bool texUV, bool texId, bool color) : RenderObjectBuilder(size, texUV, texId, color) {};
    DynamicRenderObjectBuilder(int size) : DynamicRenderObjectBuilder(size, true, true, true) {};
    DynamicRenderObjectBuilder(bool texUV, bool texId, bool color) : DynamicRenderObjectBuilder(DEFAULT_SIZE, texUV, texId, color) {};
    DynamicRenderObjectBuilder() : DynamicRenderObjectBuilder(DEFAULT_SIZE) {};
    DynamicRenderObjectBuilder(RenderObject& object) : RenderObjectBuilder(object) {};

    virtual RenderObject* getRenderObject(bool realloc);
    virtual RenderObject* getRenderObject() { return getRenderObject(true); };

};