#include "App.h"

#include <sstream>

#include "opengl.h"
#include "render/TextureManager.h"
#include "render/Texture.h"
#include "render/Shader.h"
#include "render/Font.h"
#include "gui/GuiImageElement.h"
#include "gui/GuiNinePathImageElement.h"
#include "gui/GuiButtonElement.h"
#include "gui/GuiTextBoxElement.h"
#include "gui/Screen.h"
#include "input/MouseHandler.h"
#include "input/TouchHandler.h"
#include "net/Socket.h"
#include "net/Packet.h"
#include "net/Connection.h"
#include "net/ConnectionHandler.h"
#include "net/Network.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <png/png.h>

std::string Network::GAME_NAME = "TEST";
int Network::GAME_VERSION = 1;
int Network::MIN_GAME_VERSION = 1;

class MyConnectionHandler : public ClientConnectionHandler {

public:
    MyConnectionHandler(Connection& connection) : ClientConnectionHandler(connection) {};

    virtual void connected() {
        Logger::main->debug("MyConnectionHandler", "Connected!");
    };

};

App* App::instance = null;

App::App() {
    App::instance = this;

    version.set(0, 0, 0, 1);
    version.channel = Version::Channel::DEV;
    version.netVersion = 1;
}

void App::init() {
    this->initLogger();

    Logger::main->trace("App", "Start");
    signal(SIGCHLD, SIG_IGN);

    Packet::registerCommonPackets();

    /*
    Socket socket;
    if(socket.connect("127.0.0.1", 8089, Socket::Protocol::TCP)) {
        FileBinaryStream& stream = socket.getStream();
        ConnectPacket packet;
        packet.gameName = "test";
        packet.gameVersion = version.netVersion;
        Packet::sendPacket(stream, packet);

        while(true) {
            Packet *pk = Packet::getPacket(stream);
            if(pk == null) break;
            Logger::main->debug("Socket", "Received packet: %i", pk->getId());
        }
    }*/
    Connection connection ("127.0.0.1", 8089);
    MyConnectionHandler handler (connection);
    connection.setHandler(handler);
    connection.loop();

    MouseHandler::reset();
    TouchHandler::reset();
}

MultiLogger* App::initLogger() {
    MultiLogger* logger = new MultiLogger();
    Logger::main = logger;

    return logger;
}

void App::initOpenGL() {
    TextureManager::init();

    if(testShader == null) {
        testShader = new Shader("shaders/main");
        testShader->vertexAttrib("aVertexPosition");
        testShader->colorAttrib("aColor");
        testShader->texIdAttrib("aTextureId");
        testShader->texUVAttrib("aTextureCoord");
        testShader->projectionMatrixUniform("uProjectionMatrix");
        testShader->viewMatrixUniform("uViewMatrix");
        testShader->uniform("uSamplers");
    }
    //testShader->uniform("uFragmentColor");
    //glUniform4f(testShader->uniform("uFragmentColor"), 1.0f, 1.0f, 1.0f, 1.0f);

    Font::main = new Font("images/font.png", 8, 8);
    GuiElement::initTexture();

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Texture::EMPTY->bind(0);
    GuiElement::texture->bind(1);
    Font::main->getTexture()->bind(2);

    currentScreen = new Screen(this);

    GuiImageElement* el = new GuiImageElement(10, 10, GuiElement::texture, 0, 0, 8, 8);
    currentScreen->addElement(el);

    GuiNinePathImageElement* el2 = new GuiNinePathImageElement(50, 10, 60, 20, testTexture, 0, 0, 8, 8, 2);
    currentScreen->addElement(el2);

    GuiButtonElement* btn = new GuiButtonElement(100, 100, 100, 20, "Testing");
    currentScreen->addElement(btn);

    GuiTextBoxElement* textBox = new GuiTextBoxElement(10, 100, 70, 20, "Test");
    currentScreen->addElement(textBox);

    Texture::EMPTY->unbind();
    GuiElement::texture->unbind();
    Font::main->getTexture()->unbind();
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

    if(testShader == null || testShader->getId() == 0) {
        return;
    }
    testShader->use();

    Texture::EMPTY->bind(0);
    GuiElement::texture->bind(1);
    Font::main->getTexture()->bind(2);

    GLint textures[TextureManager::MAX_TEXTURES];
    for(int i = 0; i < TextureManager::MAX_TEXTURES; i++) {
        textures[i] = i;
    }
    glUniform1iv(testShader->uniform("uSamplers"), TextureManager::MAX_TEXTURES, textures);

    glm::mat4 projection = glm::ortho(0.0f, guiWidth + 0.0f, 0.0f, guiHeight + 0.0f, 0.1f, 100.f);//glm::perspective(60.0f, (float) guiWidth / guiHeight, 0.1f, 100.0f);
    glUniformMatrix4fv(testShader->projectionMatrixUniform(), 1, false, glm::value_ptr(projection));
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    //view = glm::scale(view, glm::vec3(3.0f));
    glUniformMatrix4fv(testShader->viewMatrixUniform(), 1, false, glm::value_ptr(view));

    //testObject->render();
    this->currentScreen->render();

    Texture::EMPTY->unbind();
    GuiElement::texture->unbind();
    Font::main->getTexture()->unbind();
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