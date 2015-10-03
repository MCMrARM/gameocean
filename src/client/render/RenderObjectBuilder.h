#pragma once

#include <memory>
#include "common.h"
#include "Texture.h"
#include "RenderObject.h"
#include "Color.h"

class RenderObjectBuilder {

protected:
    int size;
    std::shared_ptr<std::vector<float>> vertexArr = null;
    std::shared_ptr<std::vector<float>> textureUVArr = null;
    std::shared_ptr<std::vector<float>> textureIdArr = null;
    std::shared_ptr<std::vector<float>> colorArr = null;
    bool renderObjectBuilt = false;

    void addVertex(float x, float y, float z, float texU, float texV, int texId, float r, float g, float b, float a);

public:
    int pos = 0;

    RenderObjectBuilder(int size, bool texUV, bool texId, bool color);
    RenderObjectBuilder(int size) : RenderObjectBuilder(size, true, true, true) {};
    RenderObjectBuilder() : RenderObjectBuilder(128, true, true, true) {};
    RenderObjectBuilder(std::shared_ptr<std::vector<float>> vertex, std::shared_ptr<std::vector<float>> texUV,
                        std::shared_ptr<std::vector<float>> texId, std::shared_ptr<std::vector<float>> color);
    RenderObjectBuilder(RenderObject& object);

    inline void vertex(float x, float y, float z, float texU, float texV, int texId, Color color) {
        addVertex(x, y, z, texU, texV, texId, color.r, color.g, color.b, color.a);
    };
    inline void vertex(float x, float y, float z, int texX, int texY, Texture* tex, Color color) {
        vertex(x, y, z, (float) texX / tex->getWidth(), (float) (tex->getHeight() - texY) / tex->getHeight(), tex->getBindTextureId(), color);
    };
    inline void vertex(float x, float y, float z, float texU, float texV, int texId) {
        vertex(x, y, z, texU, texV, texId, {1.f, 1.f, 1.f, 1.f});
    };
    inline void vertex(float x, float y, float z, int texX, int texY, Texture* tex) {
        vertex(x, y, z, texX, texY, tex->getBindTextureId(), {1.f, 1.f, 1.f, 1.f});
    };
    inline void vertex(float x, float y, float z, float texU, float texV) {
        vertex(x, y, z, texU, texV, 0, {1.f, 1.f, 1.f, 1.f});
    };
    inline void vertex(float x, float y, float z, float texU, float texV, Color color) {
        vertex(x, y, z, texU, texV, 0, color);
    };
    inline void vertex(float x, float y, float z, Color color) {
        vertex(x, y, z, 0.f, 0.f, 0, color);
    };

    void rect2d(float x, float y, float x2, float y2, float texU1, float texV1, float texU2, float texV2, int texId, Color color);
    inline void rect2d(float x, float y, float x2, float y2, float texU1, float texV1, float texU2, float texV2, int texId) {
        rect2d(x, y, x2, y2, texU1, texV1, texU2, texV2, texId, {1.f, 1.f, 1.f, 1.f});
    };
    inline void rect2d(float x, float y, float x2, float y2, int texX1, int texY1, int texX2, int texY2, Texture* tex, Color color) {
        rect2d(x, y, x2, y2, (float) texX1 / tex->getWidth(), (float) (tex->getHeight() - texY2) / tex->getHeight(),
               (float) texX2 / tex->getWidth(), (float) (tex->getHeight() - texY1) / tex->getHeight(),
               tex->getBindTextureId(), color);
    };
    inline void rect2d(float x, float y, float x2, float y2, int texX1, int texY1, int texX2, int texY2, Texture* tex) {
        rect2d(x, y, x2, y2, texX1, texY1, texX2, texY2, tex, {1.f, 1.f, 1.f, 1.f});
    };
    inline void rect2d(float x, float y, float x2, float y2, Color color) {
        rect2d(x, y, x2, y2, 0.f, 0.f, 0.f, 0.f, 0, {1.f, 1.f, 1.f, 1.f});
    };

    std::unique_ptr<RenderObject> getRenderObject();

};