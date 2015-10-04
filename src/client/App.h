#pragma once

#include "common.h"
#include <string>
#include <memory>
#include "anim/ScreenSwitchAnimation.h"

class MultiLogger;
class Texture;
class Shader;
class Screen;

class App {

    Texture* testTexture = null;
    Shader* testShader = null;
    std::shared_ptr<Screen> currentScreen = null;
    std::shared_ptr<Screen> animateScreen = null;
    std::unique_ptr<ScreenSwitchAnimation> screenAnim;

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

    inline std::shared_ptr<Screen> getScreen() { return currentScreen; };
    void setScreen(std::shared_ptr<Screen> screen) {
        currentScreen = screen;
    }
    void setScreen(std::shared_ptr<Screen> screen, std::unique_ptr<ScreenSwitchAnimation> anim);
    template<class T>
    void setScreen(std::shared_ptr<Screen> screen, float duration);

    virtual void showKeyboard(std::string text) = 0;
    virtual void hideKeyboard() = 0;

};
