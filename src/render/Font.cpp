#include "Font.h"

#include "../App.h"
#include "../utils/Logger.h"
#include "TextureManager.h"
#include "RenderObjectBuilder.h"

Font* Font::main = null;

Font::Font(std::string fontName, int charW, int charH) {
    texture = TextureManager::require(fontName);
    this->charW = charW;
    this->charH = charH;
    this->charsPerLine = texture->getWidth() / charW;
    int charsH = texture->getHeight() / charH;
    this->charSizes = new unsigned char[charsPerLine * charsH];
    unsigned int w, h;
    unsigned int byteSize = 0;
    byte* data = App::instance->readGameImageFile(fontName, w, h, byteSize);
    if(byteSize == 0) return;
    for(int i = 0; i < charsPerLine; i++) {
        for(int j = 0; j < charsH; j++) {
            int charX = charW * i;
            int charY = charH * j;
            int charSize = 0;
            while(charSize < charW) {
                bool pixel = false;
                for(int l = 0; l < charH; l++) {
                    int dPos = (h - 1 - (charY + l)) * w * 4 + (charX + charSize) * 4;//(w * (h - charY + l) + charX + charSize) * 4;//(w * (charY + l) + charX + charSize) * 4;
                    if(data[dPos + 3] > 0) { // alpha
                        pixel = true;
                        break;
                    }
                }
                if(!pixel) {
                    break;
                }
                charSize++;
            }
            //charSizes[j * charsPerLine + i] = charSize;
            Logger::main->trace("Logger", "log %i %i %i %i", i, j, i * charsH + j, charSize);
            charSizes[j * charsPerLine + i] = charSize;
        }
    }
    charSizes[32] = 4;
}

void Font::buildWordWrap(RenderObjectBuilder *builder, int x, int y, int w, std::string text, Color color) {
    bool bold = false;
    bool italic = false;
    bool underline = false;
    Logger::main->trace("Font:build", "buildstart %i", x);
    for(char& c : text) {
        unsigned char& cId = (unsigned char&) c;
        int cW = charSizes[c];

        int cX = (cId % charsPerLine) * 8;
        int cY = (cId / charsPerLine) * 8;

        builder->rect2d(x, y, x + cW, y + charH, cX, cY, cX + cW, cY + charH, texture, color);

        x += cW + 1;

        Logger::main->trace("Font:build", "build %i %i %i %i %i %i", x, cId, cX, cY, cW, charH);
    }
    Logger::main->trace("Font:build", "buildend %i", x);
}

int Font::getVertexCount(std::string text) {
    return 6 * text.length();
}