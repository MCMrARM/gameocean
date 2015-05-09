#pragma once

#include "App.h"
#include <android_native_app_glue.h>
#include <EGL/egl.h>

class AndroidApp : public App {

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    android_app* androidApp;

public:
    AndroidApp(android_app* state);

	virtual MultiLogger* initLogger();
    virtual void initOpenGL();
    virtual void destroyOpenGL();

    virtual void render();
    
    virtual byte* readGameFile(std::string name, unsigned int& size);

};
