#include "Shader.h"

#include <sstream>

#include "common.h"
#include "../App.h"
#include "TextureManager.h"

Shader* Shader::current = null;

Shader::Shader(std::string vertexShaderFile, std::string fragmentShaderFile) {
    std::string vcode = App::instance->readGameTextFile(vertexShaderFile);
    std::string fcode = App::instance->readGameTextFile(fragmentShaderFile);

    std::stringstream stream;
    stream << "#define MAX_TEXTURES " << TextureManager::MAX_TEXTURES << "\n" << fcode;
    fcode = stream.str();

    id = Shader::linkProgram(vcode, fcode);
}

void Shader::use() {
    if(this->id == 0) { Logger::main->warn("Shader", "Shader pointer is null!"); return; }

    if(Shader::current == this) {
        return;
    }
    if(Shader::current != null) {
        Shader::current->disableVertexArrays();
    }
    Shader::current = this;
    glUseProgram(this->id);
    this->enableVertexArrays();
}

int Shader::attribArray(std::string name) {
    if(this->attribs.count(name) > 0) {
        return this->attribs[name];
    }
    int loc = glGetAttribLocation(this->id, name.c_str());
    this->attribs[name] = loc;
    return loc;
}

int Shader::uniform(std::string name) {
    if(this->uniforms.count(name) > 0) {
        return this->uniforms[name];
    }
    int loc = glGetUniformLocation(this->id, name.c_str());
    if(loc == -1) {
        Logger::main->warn("Shader", "Uniform %s is unknown!", name.c_str());
        return -1;
    }

    this->uniforms[name] = loc;
    return loc;
}

void Shader::enableVertexArrays() {
    for(const std::pair<std::string, int>& entry : attribs) {
        Logger::main->trace("Shader", "Enabling vertex array: %s (%i)", entry.first.c_str(), entry.second);
        glEnableVertexAttribArray(entry.second);
    }
}
void Shader::disableVertexArrays() {
    for(const std::pair<std::string, int>& entry : attribs) {
        glDisableVertexAttribArray(entry.second);
    }
}

unsigned int Shader::compileShader(ShaderType type, std::string code) {
    GLuint shader = glCreateShader((GLenum) type);
    const GLchar* scode = (const GLchar*) code.c_str();
    glShaderSource(shader, 1, &scode, 0);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        std::string strType ("UNKNOWN");
        if(type == ShaderType::VERTEX) {
            strType = "vertex";
        } else if(type == ShaderType::FRAGMENT) {
            strType = "fragment";
        }

        Logger::main->error("Shader", "Couldn't compile %s shader", strType.c_str());

        GLint size = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);

        Logger::main->error("Shader", "Error log [%i]", size);

        GLchar errors[size];
        glGetShaderInfoLog(shader, size, &size, errors);

        Logger::main->error("Shader", errors);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

unsigned int Shader::linkProgram(std::string vertexCode, std::string fragmentCode) {
    unsigned int vertexShader = Shader::compileShader(ShaderType::VERTEX, vertexCode);
    if(vertexShader == 0) { return 0; }
    unsigned int fragmentShader = Shader::compileShader(ShaderType::FRAGMENT, fragmentCode);
    if(fragmentShader == 0) { return 0; }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(success == GL_FALSE) {
        Logger::main->error("Shader", "Couldn't link shaders");

        GLint size = 0;
        glGetShaderiv(program, GL_INFO_LOG_LENGTH, &size);

        Logger::main->error("Shader", "Error log [%i]", size);

        GLchar errors[size];
        glGetShaderInfoLog(program, size, &size, errors);

        Logger::main->error("Shader", errors);

        glDeleteProgram(program);
        return 0;
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    return program;
}