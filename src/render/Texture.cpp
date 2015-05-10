#include "Texture.h"

#include "opengl.h"
#include "../common.h"
#include "TextureManager.h"
#include "../App.h"

Texture::Texture() {
    //
}

Texture::Texture(std::string name) {
    this->load(name);
}

Texture::~Texture() {
    GLuint tex[1];
    tex[0] = id;
    glDeleteTextures(1, &tex[0]);

    Logger::main->trace("Texture", "Destroyed texture: %i", id);
}

void Texture::load(std::string name) {
    GLuint tex[1];
    glGenTextures(1, &tex[0]);
    glBindTexture(GL_TEXTURE_2D, tex[0]);

    Logger::main->trace("Texture", "Initialized texture: %i", tex[0]);

    this->id = tex[0];

    /*
    GLint data[] = {255, 0, 0, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    */

    unsigned int w, h, size;
    GLbyte* data = (GLbyte*) App::instance->readGameImageFile(name, w, h, size);
    Logger::main->debug("Texture", "Loaded! [w: %i, h: %i, byte size: %i] %i", w, h, size, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
    delete[] data;
}

int Texture::bind() {
    for(int i = 0; i < TextureManager::MAX_TEXTURES; i++) {
        Texture* tex = TextureManager::activeTextures[i];
        if(tex == null || tex->bindId == -1) {
            this->bind(i);
            return i;
        }
    }

    return -1;
}

void Texture::bind(int texId) {
    this->bindId = texId;
    glActiveTexture(GL_TEXTURE0 + texId);
    glBindTexture(GL_TEXTURE_2D, this->id);
}

void Texture::unbind() {
    this->bindId = -1;
}