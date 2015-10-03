#include <GLFW/glfw3.h>
#include "LinuxApp.h"
#include "utils/ResourceManager.h"

LinuxApp* app;

int main(int argc, char **argv) {
    ResourceManager::instance = new FileResourceManager("assets/", ".");
    app = new LinuxApp();
    app->init();

    if (!glfwInit()) {
        Logger::main->error("GLFW", "Call to glfwInit failed");
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(640, 480, "client", NULL, NULL);
    app->resize(640, 480);
    if (!window)
    {
        Logger::main->error("GLFW", "Couldn't create window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, LinuxApp::glfwKeyCallback);
    glfwSetCharCallback(window, LinuxApp::glfwCharCallback);
    glfwSetCursorPosCallback(window, LinuxApp::glfwMousePosCallback);
    glfwSetMouseButtonCallback(window, LinuxApp::glfwMouseButtonCallback);

    app->initOpenGL();
    while (!glfwWindowShouldClose(window))
    {
        app->render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
/*
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowPosition(50, 25);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("lbclient");
*/
    //glutDisplayFunc(draw);
    //glutMainLoop();
    return 0;
}