#pragma once

#include <memory>
class Texture;

struct TextureCoords {
    std::shared_ptr<Texture> texture;
    float u1, v1, u2, v2;
};