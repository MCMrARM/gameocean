#pragma once

#include <string>
class Texture;
class RenderObjectBuilder;
#include "Color.h"

class Font {

    Texture* texture;
    int charW, charH;
    int charsPerLine;
    unsigned char* charSizes;
public:
    static Font* main;

    Font(std::string fontName, int charW, int charH);

    inline Texture* getTexture() { return texture; };

    void buildWordWrap(RenderObjectBuilder* builder, int x, int y, int w, std::string text, Color color);
    inline void build(RenderObjectBuilder* builder, int x, int y, std::string text, Color color) {
        Font::buildWordWrap(builder, x, y, -1, text, color);
    };

    int getWidth(std::string text);
    int getVertexCount(std::string text);

};


