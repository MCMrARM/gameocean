#pragma once

#include "opengl.h"

class RenderObject {

    unsigned int vertexBufferId = 0;
    unsigned int texUVBufferId = 0;
    unsigned int texIdBufferId = 0;
    unsigned int colorBufferId = 0;
public:
    int bufferType = GL_STATIC_DRAW;
    int drawType = GL_TRIANGLES;

    int size = -1;
    float *vertexArray;
    float *textureUVArray;
    float *textureIdArray;
    float *colorArray;

    RenderObject(int size, float *vertex, float *texUV, float *texId, float *color) : size(size), vertexArray(vertex),
                                                                                      textureUVArray(texUV),
                                                                                      textureIdArray(texId),
                                                                                      colorArray(color) { };
    ~RenderObject();

    void update();

    void render();

};