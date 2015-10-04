#pragma once

#include <string>
#include <map>
#include "opengl.h"

enum class ShaderType {
    VERTEX = GL_VERTEX_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER
};

class Shader {

    int id;

    std::map<std::string, int> attribs;
    std::map<std::string, int> uniforms;

    int sVertexAttrib = -1;
    int sTexUVAttrib = -1;
    int sTexIdAttrib = -1;
    int sColorAttrib = -1;
    int sProjectionMatrixUniform = -1;
    int sViewMatrixUniform = -1;
    int sColorUniform = -1;

public:
    static Shader* current;

    Shader(std::string vertexShaderFile, std::string fragmentShaderFile);
    Shader(std::string name) : Shader(name + ".vertex", name + ".fragment") {};

    inline int getId() { return id; }

    void use();
    void enableVertexArrays();
    void disableVertexArrays();

    int attribArray(std::string name);
    int uniform(std::string name);

    void vertexAttrib(std::string name) { sVertexAttrib = attribArray(name); };
    void texUVAttrib(std::string name) { sTexUVAttrib = attribArray(name); };
    void texIdAttrib(std::string name) { sTexIdAttrib = attribArray(name); };
    void colorAttrib(std::string name) { sColorAttrib = attribArray(name); };
    inline int vertexAttrib() { return sVertexAttrib; };
    inline int texUVAttrib() { return sTexUVAttrib; };
    inline int texIdAttrib() { return sTexIdAttrib; };
    inline int colorAttrib() { return sColorAttrib; };

    void projectionMatrixUniform(std::string name) { sProjectionMatrixUniform = uniform(name); }
    void viewMatrixUniform(std::string name) { sViewMatrixUniform = uniform(name); }
    inline int projectionMatrixUniform() { return sProjectionMatrixUniform; };
    inline int viewMatrixUniform() { return sViewMatrixUniform; };

    void colorUniform(std::string name) { sColorUniform = uniform(name); };
    inline int colorUniform() { return sColorUniform; };

    static unsigned int compileShader(ShaderType type, std::string code);
    static unsigned int linkProgram(std::string vertexCode, std::string fragmentCode);

};