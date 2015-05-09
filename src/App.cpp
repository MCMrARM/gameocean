#include "App.h"

#include <sstream>

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

#include <png/png.h>

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

    testTexture = TextureManager::require("images/test.png");
    testShader = new Shader("shaders/main");
    testShader->vertexAttrib("aVertexPosition");
    testShader->colorAttrib("aColor");
    testShader->texUVAttrib("aTextureCoord");
    testShader->projectionMatrixUniform("uProjectionMatrix");
    testShader->viewMatrixUniform("uViewMatrix");
    testShader->uniform("uSampler");
    //testShader->uniform("uFragmentColor");
    //glUniform4f(testShader->uniform("uFragmentColor"), 1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_CULL_FACE);

    StaticRenderObjectBuilder builder (6, true, false, true);/*
    builder.vertex(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    builder.vertex(270.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    builder.vertex(0.0f, 270.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);*/
    //builder.rect2d(0, 0, 200, 200, 1.0f, 0.0f, 0.0f, 1.0f);
    builder.rect2d(0, 0, 200, 200, 0, 0, 1, 1, 0, 1, 1, 1, 1);
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
    if(this->testTexture != null) {
        testShader->use();

        int texId = this->testTexture->bind();
        glUniform1i(testShader->uniform("uSampler"), texId);

        glm::mat4 projection = glm::ortho(0.0f, guiWidth + 0.0f, 0.0f, guiHeight + 0.0f, 0.1f, 100.f);//glm::perspective(60.0f, (float) guiWidth / guiHeight, 0.1f, 100.0f);
        glUniformMatrix4fv(testShader->projectionMatrixUniform(), 1, false, glm::value_ptr(projection));
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        glUniformMatrix4fv(testShader->viewMatrixUniform(), 1, false, glm::value_ptr(view));

        testObject->render();

        this->testTexture->unbind();
    }

}

std::string App::readGameTextFile(std::string name) {
    unsigned int size = 0;
    byte* data = this->readGameFile(name, size);

    std::string str ((char*) data, size);
    delete data;
    return str;
}

void readGameImageFile_callback(png_structp png_ptr, png_bytep out, png_size_t size) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if(!io_ptr) return;
    std::istringstream* dataStream = (std::istringstream*) io_ptr;
    dataStream->read((char*) out, size);
}

byte* App::readGameImageFile(std::string name, unsigned int& width, unsigned int& height, unsigned int& byteSize) {
    unsigned int size = 0;
    byte* data = this->readGameFile(name, size);

    std::istringstream dataStream (std::string((char*) data, size));
    delete data;

    char sig [8];
    dataStream.read(&sig[0], 8);
    int i = png_sig_cmp((png_bytep) sig, 0, 8);
    if(i != 0) {
        Logger::main->warn("Image", "File %s is not a png image [%i]", name.c_str(), i);
        return null;
    }

    png_struct* png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, null, null, null);
    if(!png_ptr) return null;

    png_info* info_ptr = png_create_info_struct(png_ptr);

    png_set_read_fn(png_ptr, (void*) &dataStream, readGameImageFile_callback);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 iwidth, iheight;
    int depth, colorType;
    png_get_IHDR(png_ptr, info_ptr, &iwidth, &iheight, &depth, &colorType, null, null, null);

    width = (unsigned int) iwidth;
    height = (unsigned int) iheight;

    if(colorType != PNG_COLOR_TYPE_RGBA) {
        Logger::main->warn("Image", "%s: color type (%i) is not RGBA", name.c_str(), colorType);
    }

    png_size_t rowBytes = png_get_rowbytes(png_ptr, info_ptr);

    byteSize = (unsigned int) rowBytes * height;

    //png_byte* rows [height];
    png_byte* rows [height];
    byte* img = new byte[rowBytes * height];

    for(int i = 0; i < height; i++) {
        rows[i] = (png_byte*) &img[(height - 1 - i) * rowBytes];
    }

    png_read_image(png_ptr, rows);

    png_destroy_read_struct(&png_ptr, &info_ptr, null);

    return img;
}