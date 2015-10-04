#include "App.h"

#include <sstream>
#include <csignal>

#include "opengl.h"
#include "render/TextureManager.h"
#include "render/Texture.h"
#include "render/Shader.h"
#include "render/Font.h"
#include "GameInfo.h"
#include "gui/GuiImageElement.h"
#include "gui/GuiNinePathImageElement.h"
#include "gui/GuiButtonElement.h"
#include "gui/GuiTextBoxElement.h"
#include "gui/Screen.h"
#include "anim/Animation.h"
#include "anim/GuiAnimationProperty.h"
#include "anim/ScreenSwitchAnimation.h"
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

    GameInfo::current = new GameInfo("lbsg", Version(0, 0, 0, 1));
    GameInfo::current->version.setNetVersion(1, 1);
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
    /*
    Connection connection ("127.0.0.1", 8089);
    MyConnectionHandler handler (connection);
    connection.setHandler(handler);
    connection.loop();*/

    MouseHandler::reset();
    TouchHandler::reset();
}

MultiLogger* App::initLogger() {
    MultiLogger* logger = new MultiLogger();
    Logger::main = logger;

    return logger;
}

void App::setScreen(std::shared_ptr<Screen> screen, std::unique_ptr<ScreenSwitchAnimation> anim) {
    this->animateScreen = screen;
    this->screenAnim = std::move(anim);
}

template<class T>
void App::setScreen(std::shared_ptr<Screen> screen, float duration) {
    animateScreen = screen;
    screenAnim = std::unique_ptr<ScreenSwitchAnimation>(new T(currentScreen, animateScreen, duration));
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
        testShader->colorUniform("uFragmentColor");
        testShader->uniform("uSamplers");
    }

    Font::main = new Font("images/font.png", 8, 8);
    GuiElement::initTexture();

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Texture::EMPTY->bind(0);
    GuiElement::texture->bind(1);
    Font::main->getTexture()->bind(2);

    std::shared_ptr<Screen> mainScreen = std::shared_ptr<Screen>(new Screen(this));

    /*GuiImageElement* el = new GuiImageElement(10, 10, GuiElement::texture, 0, 0, 8, 8);
    currentScreen->addElement(el);

    GuiNinePathImageElement* el2 = new GuiNinePathImageElement(50, 10, 60, 20, testTexture, 0, 0, 8, 8, 2);
    currentScreen->addElement(el2);*/

    std::shared_ptr<GuiButtonElement> btn (new GuiButtonElement(100, 100, 100, 20, "Testing"));
    btn->setClickCallback([this, btn, mainScreen](GuiButtonElement* el) {
        Logger::main->debug("Test", "Clicked %i", testShader->uniform("uFragmentColor"));

        std::shared_ptr<Screen> test (new Screen(this));

        std::shared_ptr<GuiButtonElement> btn2 (new GuiButtonElement(10, 10, 50, 50, "Close"));
        btn2->setClickCallback([this, btn, btn2, mainScreen](GuiButtonElement* el) {
            setScreen(mainScreen, std::unique_ptr<ScreenSwitchAnimation>(new MoveElementScreenSwitchAnimation(currentScreen, mainScreen, btn2, btn, 1.f)));
        });
        test->addElement(btn2);

        //setScreen<FadeScreenSwitchAnimation>(test, 1.f);
        setScreen(test, std::unique_ptr<ScreenSwitchAnimation>(new MoveElementScreenSwitchAnimation(currentScreen, test, btn, btn2, 1.f)));

        //currentScreen->animations.push_back(std::unique_ptr<Animation>(new PropertyAnimation(std::unique_ptr<GuiAnimationProperty>(new GuiPosAnimationProperty(*el, 200, (el->getY() == 150 ? 100 : 150))), 0.5f, Animation::Easing::SWING)));
    });
    mainScreen->addElement(btn);

    //currentScreen->animations.push_back(std::unique_ptr<Animation>(new PropertyAnimation(std::unique_ptr<GuiAnimationProperty>(new GuiPosAnimationProperty(*btn, 200, 100)), 1.f)));

    std::shared_ptr<GuiTextBoxElement> textBox (new GuiTextBoxElement(10, 100, 70, 20, "Test"));
    mainScreen->addElement(textBox);

    setScreen(mainScreen);

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

    glUniform4f(testShader->uniform("uFragmentColor"), 1.0f, 1.0f, 1.0f, 1.0f);

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
    if (this->screenAnim) {
        if (this->screenAnim->tick()) {
            this->currentScreen = this->animateScreen;
            this->screenAnim.reset();
        } else {
            this->animateScreen->render();
        }
    }
    this->currentScreen->render();
    if (this->screenAnim) {
        this->screenAnim->render();
    }

    Texture::EMPTY->unbind();
    GuiElement::texture->unbind();
    Font::main->getTexture()->unbind();
}

/*
std::string App::readGameTextFile(std::string name) {
    unsigned int size = 0;
    byte* data = this->readGameFile(name, size);

    std::string str ((char*) data, size);
    delete data;
    return str;
}
*/
