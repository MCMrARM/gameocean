#pragma once

class MultiLogger;

class App {

public:
    bool active = false;
    int screenWidth, screenHeight;
    int guiWidth, guiHeight;
    int pixelSize = 1;

    virtual void init();
    virtual MultiLogger* initLogger();
    virtual void initOpenGL() = 0;
    virtual void destroyOpenGL() = 0;

    virtual void render();

};
