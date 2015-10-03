#pragma once

#include "common.h"
#include <string>

class MultiLogger;
class Texture;
class Shader;
class Screen;

class App {

    Texture* testTexture = null;
    Shader* testShader = null;
    Screen* currentScreen = null;

public:
    static App* instance;

    App();

    bool active = false;
    int screenWidth, screenHeight;
    int guiWidth, guiHeight;
    int pixelSize = 2;

    virtual void init();
    virtual MultiLogger* initLogger();
    virtual void initOpenGL();
    virtual void destroyOpenGL() = 0;

    virtual void resize(int newWidth, int newHeight);

    virtual void render();

    inline Screen* getScreen() { return currentScreen; };

    virtual void showKeyboard(std::string text) = 0;
    virtual void hideKeyboard() = 0;

};
