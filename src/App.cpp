#include "App.h"

#include "opengl.h"
#include "render/TextureManager.h"
#include "render/Texture.h"
#include "render/Shader.h"
#include "render/RenderObject.h"
#include "render/RenderObjectBuilder.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

App* App::instance = null;

App::App() {
    App::instance = this;
}

void App::init() {
    this->initLogger();

    Logger::main->trace("App", "Start");
}

MultiLogger* App::initLogger() {
    MultiLogger* logger = new MultiLogger();
    Logger::main = logger;

    return logger;
}

void App::initOpenGL() {
    TextureManager::init();

    testTexture = TextureManager::require("test.png");
    testShader = new Shader("shaders/color");
    testShader->vertexAttrib("aVertexPosition");
    testShader->colorAttrib("aColor");
    testShader->projectionMatrixUniform("uProjectionMatrix");
    testShader->viewMatrixUniform("uViewMatrix");

    glEnable(GL_CULL_FACE);

    StaticRenderObjectBuilder builder (6, false, false, true);/*
    builder.vertex(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    builder.vertex(270.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    builder.vertex(0.0f, 270.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);*/
    builder.rect2d(0, 0, 200, 200, 1.0f, 0.0f, 0.0f, 1.0f);
    static RenderObject object = builder.getRenderObject();
    this->testObject = &object;
}

void App::resize(int newWidth, int newHeight) {
    screenWidth = newWidth;
    screenHeight = newHeight;
    guiWidth = newWidth / pixelSize;
    guiHeight = newHeight / pixelSize;

    Logger::main->debug("App", "New screen size: %i %i; GUI size: %i %i (pixel size: %i)", screenWidth, screenHeight, guiWidth, guiHeight, pixelSize);
}

void App::render() {
    glViewport(0, 0, screenWidth, screenHeight);

    glClearColor(0.5, 0.7, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if(testShader->getId() == 0) {
        return;
    }

    testShader->use();
    glm::mat4 projection = glm::ortho(0.0f, guiWidth + 0.0f, 0.0f, guiHeight + 0.0f, 0.1f, 100.f);//glm::perspective(60.0f, (float) guiWidth / guiHeight, 0.1f, 100.0f);
    glUniformMatrix4fv(testShader->projectionMatrixUniform(), 1, false, glm::value_ptr(projection));
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glUniformMatrix4fv(testShader->viewMatrixUniform(), 1, false, glm::value_ptr(view));

    testObject->render();

    /*
    if(this->testTexture != null) {
        this->testTexture->bind();

        this->testTexture->unbind();
    }*/


}

std::string App::readGameTextFile(std::string name) {
    unsigned int size = 0;
    byte* data = this->readGameFile(name, size);

    std::string str ((char*) data, size);
    return str;
}