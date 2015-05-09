#pragma once

#include <string>

class Texture {

    std::string name = "None";
    unsigned int id;
    unsigned int bindId;

public:
    Texture();
    Texture(std::string name);
    ~Texture();

    inline std::string getName() { return name; };
    inline int getBindTextureId() { return bindId; };

    void load(std::string name);

    int bind();
    void bind(int texId);
    void unbind();

};