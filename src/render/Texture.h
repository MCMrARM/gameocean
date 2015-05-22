#pragma once

#include <string>
#include "../common.h"

class Texture {

    std::string name = "None";
    unsigned int id;
    unsigned int bindId = -1;
    unsigned int width, height;

public:
    static Texture* EMPTY;

    Texture();
    Texture(std::string name);
    ~Texture();

    inline std::string getName() { return name; };
    inline int getBindTextureId() { return bindId; };
    inline unsigned int getWidth() { return width; };
    inline unsigned int getHeight() { return height; };

    void load(); // loads a white 1x1 texture
    void load(std::string name);
    void load(byte* data, int w, int h); // requires texture to be in RGBA format

    int bind();
    void bind(int texId);
    void unbind();

};