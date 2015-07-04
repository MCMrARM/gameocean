#include "RenderObject.h"

#include "common.h"
#include "Shader.h"

RenderObject::~RenderObject() {
    GLuint buffer[1];
    if(vertexBufferId != 0) {
        buffer[0] = vertexBufferId;
        glDeleteBuffers(1, buffer);
    }
    if(texUVBufferId != 0) {
        buffer[0] = texUVBufferId;
        glDeleteBuffers(1, buffer);
    }
    if(texIdBufferId != 0) {
        buffer[0] = texIdBufferId;
        glDeleteBuffers(1, buffer);
    }
    if(colorBufferId != 0) {
        buffer[0] = colorBufferId;
        glDeleteBuffers(1, buffer);
    }
}

void RenderObject::render() {
    if(Shader::current == null) { Logger::main->warn("Shader", "Tried to render a object without a shader set!"); return; }

    if(vertexBufferId == 0) { Logger::main->warn("RenderObject", "Tried to render a object but the vertex array is null!"); return; }

    int vertexAttrib = Shader::current->vertexAttrib();
    if(vertexAttrib == -1) { Logger::main->warn("Shader", "Vertex attribute not definied in the current shader!"); return; }
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, false, 0, 0);

    if(texUVBufferId != 0) {
        int texUVAttrib = Shader::current->texUVAttrib();
        if(texUVAttrib == -1) { Logger::main->warn("Shader", "Texture UV attribute not definied in the current shader!"); }
        glBindBuffer(GL_ARRAY_BUFFER, texUVBufferId);
        glVertexAttribPointer(texUVAttrib, 2, GL_FLOAT, false, 0, 0);
    }

    if(texIdBufferId != 0) {
        int texIdAttrib = Shader::current->texIdAttrib();
        if(texIdAttrib == -1) { Logger::main->warn("Shader", "Texture ID attribute not definied in the current shader!"); }
        glBindBuffer(GL_ARRAY_BUFFER, texIdBufferId);
        glVertexAttribPointer(texIdAttrib, 1, GL_FLOAT, false, 0, 0);
    }

    if(colorBufferId != 0) {
        int colorAttrib = Shader::current->colorAttrib();
        if(colorAttrib == -1) { Logger::main->warn("Shader", "Color attribute not definied in the current shader!"); }
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferId);
        glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, false, 0, 0);
    }

    glDrawArrays(drawType, 0, size);
}

void RenderObject::update() {
    if(vertexBufferId == 0) {
        GLuint buffer[1];
        glGenBuffers(1, &buffer[0]);
        vertexBufferId = buffer[0];
    }
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, size * 3 * 4, vertexArray, bufferType);

    if(textureUVArray != null) {
        if (texUVBufferId == 0) {
            GLuint buffer[1];
            glGenBuffers(1, &buffer[0]);
            texUVBufferId = buffer[0];
        }
        glBindBuffer(GL_ARRAY_BUFFER, texUVBufferId);
        glBufferData(GL_ARRAY_BUFFER, size * 2 * 4, textureUVArray, bufferType);
    }

    if(textureIdArray != null) {
        if (texIdBufferId == 0) {
            GLuint buffer[1];
            glGenBuffers(1, &buffer[0]);
            texIdBufferId = buffer[0];
        }
        glBindBuffer(GL_ARRAY_BUFFER, texIdBufferId);
        glBufferData(GL_ARRAY_BUFFER, size * 4, textureIdArray, bufferType);
    }

    if(colorArray != null) {
        if(colorBufferId == 0) {
            GLuint buffer[1];
            glGenBuffers(1, &buffer[0]);
            colorBufferId = buffer[0];
        }
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferId);
        glBufferData(GL_ARRAY_BUFFER, size * 4 * 4, colorArray, bufferType);
    }
}

void RenderObject::updateFragment(int offset, int size, bool vertex, bool texUV, bool texId, bool color) {
    if(vertex && vertexArray != null) {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        glBufferSubData(GL_ARRAY_BUFFER, offset * 3 * sizeof(float), size * 3 * sizeof(float), &vertexArray[offset * 3]);
    }
    if(texUV && textureUVArray != null) {
        glBindBuffer(GL_ARRAY_BUFFER, texUVBufferId);
        glBufferSubData(GL_ARRAY_BUFFER, offset * 2 * sizeof(float), size * 2 * sizeof(float), &textureUVArray[offset * 2]);
    }
    if(texId && textureIdArray != null) {
        glBindBuffer(GL_ARRAY_BUFFER, texIdBufferId);
        glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(float), size * sizeof(float), &textureIdArray[offset]);
    }
    if(color && colorArray != null) {
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferId);
        glBufferSubData(GL_ARRAY_BUFFER, offset * 4 * sizeof(float), size * 4 * sizeof(float), &colorArray[offset * 4]);
    }
}