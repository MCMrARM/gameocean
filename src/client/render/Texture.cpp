#include "Texture.h"

#include "opengl.h"
#include "TextureManager.h"
#include "utils/ResourceManager.h"
#include "../App.h"

Texture* Texture::EMPTY = null;

Texture::Texture() {
    GLuint tex[1];
    glGenTextures(1, &tex[0]);
    glBindTexture(GL_TEXTURE_2D, tex[0]);

    Logger::main->trace("Texture", "Initialized texture: %i", tex[0]);

    this->id = tex[0];
}

Texture::Texture(std::string name) : Texture() {
    this->load(name);
}

Texture::~Texture() {
    GLuint tex[1];
    tex[0] = id;
    glDeleteTextures(1, &tex[0]);

    Logger::main->trace("Texture", "Destroyed texture: %i", id);
}

void Texture::load() {
    GLubyte data[] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

    width = 1;
    height = 1;
}

void Texture::load(byte *data, int w, int h) {
    GLbyte* glData = (GLbyte*) data;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &glData[0]);

    width = w;
    height = h;
}

void Texture::load(std::string name) {
    ResourceManager::PNGInfo data = ResourceManager::instance->readAssetImageFile(name);
    Logger::main->debug("Texture", "Loaded! [w: %i, h: %i, byte size: %i]", data.width, data.height, data.dataSize);

    Texture::load(&(*data.data)[0], data.width, data.height);
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