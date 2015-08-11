#include "LinuxApp.h"

#include "input/KeyboardHandler.h"
#include "input/MouseHandler.h"
#include "opengl.h"
#include <fstream>
#include <GLFW/glfw3.h>

std::string LinuxApp::currentInputText = std::string ();

MultiLogger* LinuxApp::initLogger() {
    MultiLogger* r = App::initLogger();
    r->addLogger(new Logger());
    return r;
}

void LinuxApp::initOpenGL() {
    GLenum err = glewInit();
    if (err != GLEW_OK)
        exit(1);
    if (!GLEW_VERSION_2_1)
        exit(1);

    App::initOpenGL();
}

void LinuxApp::destroyOpenGL() {
    //
}

byte* LinuxApp::readGameFile(std::string name, unsigned int &size) {
    Logger::main->trace("LinuxApp/Assets", "Loading: %s", name.c_str());
    std::ifstream ifs ("assets/" + name, std::ios::binary | std::ios::ate);
    if (!ifs) {
        Logger::main->error("LinuxApp/Assets", "Failed to load: %s", name.c_str());
        size = 0;
        return null;
    }
    size = (int) ifs.tellg();

    byte* ret = new byte[size];
    ifs.seekg(0, std::ios::beg);
    ifs.read((char*) &ret[0], size);
    return ret;
}

void LinuxApp::glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
        if (LinuxApp::currentInputText.length() > 0) {
            LinuxApp::currentInputText.pop_back();
            KeyboardHandler::inputSetText(LinuxApp::currentInputText);
        }
    }

    if (key < 0 || key >= KeyboardHandler::MAX_KEYS)
        return;

    if (action == GLFW_PRESS) {
        KeyboardHandler::press(key);
    } else if (action == GLFW_RELEASE) {
        KeyboardHandler::release(key);
    }
}

void LinuxApp::glfwCharCallback(GLFWwindow* window, unsigned int codepoint) {
    if (codepoint >= 256)
        return;
    LinuxApp::currentInputText += (char) codepoint;
    KeyboardHandler::inputSetText(LinuxApp::currentInputText);
}

void LinuxApp::glfwMousePosCallback(GLFWwindow *window, double xpos, double ypos) {
    MouseHandler::move((int) xpos, (int) ypos);
}

void LinuxApp::glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        MouseHandler::press(button);
    } else if (action == GLFW_RELEASE) {
        MouseHandler::release(button);
    }
}