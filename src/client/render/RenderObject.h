#pragma once

#include "opengl.h"

#include <memory>
#include <vector>

class RenderObject {

    unsigned int vertexBufferId = 0;
    unsigned int texUVBufferId = 0;
    unsigned int texIdBufferId = 0;
    unsigned int colorBufferId = 0;
public:
    int bufferType = GL_STATIC_DRAW;
    int drawType = GL_TRIANGLES;

    int size = -1;
    std::shared_ptr<std::vector<float>> vertexArray;
    std::shared_ptr<std::vector<float>> textureUVArray;
    std::shared_ptr<std::vector<float>> textureIdArray;
    std::shared_ptr<std::vector<float>> colorArray;

    RenderObject(std::shared_ptr<std::vector<float>> vertex, std::shared_ptr<std::vector<float>> texUV,
                 std::shared_ptr<std::vector<float>> texId, std::shared_ptr<std::vector<float>> color) : size(vertex->size() / 3),
                                                                                                         vertexArray(vertex),
                                                                                                         textureUVArray(texUV),
                                                                                                         textureIdArray(texId),
                                                                                                         colorArray(color) { };
    ~RenderObject();

    void update();

    void updateFragment(int offset, int size, bool vertex, bool texUV, bool texId, bool color);

    void render();

};