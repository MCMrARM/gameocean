#pragma once

#include "App.h"

class GLFWwindow;

class LinuxApp : public App {

public:
    LinuxApp() : App() { };

    virtual MultiLogger* initLogger();

    virtual void initOpenGL();
    virtual void destroyOpenGL();
    virtual byte* readGameFile(std::string name, unsigned int& size);

    static std::string currentInputText;
    virtual void showKeyboard(std::string text) {
        currentInputText = text;
    };
    virtual void hideKeyboard() {
        currentInputText = "";
    };

    static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfwCharCallback(GLFWwindow* window, unsigned int codepoint);

    static void glfwMousePosCallback(GLFWwindow* window, double xpos, double ypos);
    static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

};


