#include "AndroidApp.h"

#include "utils/Logger.h"
#include "AndroidLogger.h"

AndroidApp::AndroidApp(android_app* state) {
    this->androidApp = state;
}

MultiLogger* AndroidApp::initLogger() {
	MultiLogger* logger = App::initLogger();
	logger->addLogger(new AndroidLogger());
	return logger;
}

void AndroidApp::initOpenGL() {
    Logger::main->trace("AndroidApp", "Initializing EGL context...");

    const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    Logger::main->debug("AndroidApp", "EGL Format: %i %i", format, androidApp->window);
    ANativeWindow_setBuffersGeometry(androidApp->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, androidApp->window, NULL);

    context = eglCreateContext(display, config, NULL, contextAttribs);

    if(eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        Logger::main->error("AndroidApp", "Unable to eglMakeCurrent");
        return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &this->screenWidth);
    eglQuerySurface(display, surface, EGL_HEIGHT, &this->screenHeight);

    Logger::main->trace("AndroidApp", "EGL has been initialized!");

}

void AndroidApp::destroyOpenGL() {
    Logger::main->trace("AndroidApp", "Destroying EGL context...");

    if(display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if(context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
            context = EGL_NO_CONTEXT;
        }
        if(surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
            surface = EGL_NO_SURFACE;
        }
        eglTerminate(display);
        display = EGL_NO_DISPLAY;
    }
}

void AndroidApp::render() {
    App::render();
    eglSwapBuffers(display, surface);
}