#pragma once

#include <string>

class Texture {

    std::string name = "None";
    unsigned int id;
    unsigned int bindId;
    unsigned int width, height;

public:
    Texture();
    Texture(std::string name);
    ~Texture();

    inline std::string getName() { return name; };
    inline int getBindTextureId() { return bindId; };
    inline unsigned int getWidth() { return width; };
    inline unsigned int getHeight() { return height; };

    void load(std::string name);

    int bind();
    void bind(int texId);
    void unbind();

};