#pragma once

#include "common.h"
#include <string>

class MultiLogger;
class Texture;
class Shader;
class RenderObject;
class GuiElementContainer;
class GuiImageElement;

class App {

    Texture* testTexture;
    Shader* testShader;
    //RenderObject* testObject;
    GuiElementContainer* container;
    GuiImageElement * el;

public:
    static App* instance;

    App();

    bool active = false;
    int screenWidth, screenHeight;
    int guiWidth, guiHeight;
    int pixelSize = 8;

    virtual void init();
    virtual MultiLogger* initLogger();
    virtual void initOpenGL();
    virtual void destroyOpenGL() = 0;

    virtual void resize(int newWidth, int newHeight);

    virtual void render();

    virtual byte* readGameFile(std::string name, unsigned int& size) = 0;
    virtual std::string readGameTextFile(std::string name);
    virtual byte* readGameImageFile(std::string name, unsigned int& width, unsigned int& height, unsigned int& byteSize);

};
