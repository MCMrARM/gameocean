#include "App.h"

#include "opengl.h"
#include "utils/Logger.h"

void App::init() {
    this->initLogger();

    Logger::main->trace("App", "Start");
}

MultiLogger* App::initLogger() {
    MultiLogger* logger = new MultiLogger();
    Logger::main = logger;

    return logger;
}

void App::render() {
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}