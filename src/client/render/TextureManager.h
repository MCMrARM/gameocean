#pragma once

#include <string>
#include <vector>
#include "common.h"

class Texture;

class TextureManager {

public:
    static int MAX_TEXTURES;

    static std::vector<Texture*>* textures;
    static Texture** activeTextures;

    static void init();

    static Texture* require(std::string name);
    static void unload(Texture* texture);

};