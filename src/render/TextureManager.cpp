#include "TextureManager.h"

#include <algorithm>
#include "opengl.h"
#include "../common.h"
#include "Texture.h"

int TextureManager::MAX_TEXTURES = 0;
std::vector<Texture*>* TextureManager::textures = new std::vector<Texture*>();
Texture** TextureManager::activeTextures = null;

void TextureManager::init() {
    GLint res[1];
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, res);
    TextureManager::MAX_TEXTURES = res[0];
    Logger::main->debug("TextureManager", "Max textures: %i", TextureManager::MAX_TEXTURES);
    TextureManager::activeTextures = new Texture*[TextureManager::MAX_TEXTURES];
}

Texture* TextureManager::require(std::string name) {
    for(Texture* tex : *TextureManager::textures) {
        if(tex->getName() == name) {
            return tex;
        }
    }

    Texture* tex = new Texture(name);
    textures->push_back(tex);
}

void TextureManager::unload(Texture* texture) {
    std::remove(TextureManager::textures->begin(), TextureManager::textures->end(), texture);
    delete texture;
}