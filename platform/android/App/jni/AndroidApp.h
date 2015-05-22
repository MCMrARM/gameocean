#pragma once

#include "App.h"
#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include "JNIHelper.h"

class AndroidApp : public App {

    static EGLDisplay display;
    static EGLSurface surface;
    static EGLContext context;
    static EGLConfig eglConfig;
    android_app* androidApp;

public:
    JNIHelper jni;
    
    AndroidApp(android_app* state);

	virtual MultiLogger* initLogger();
    virtual void initOpenGL();
    virtual void destroyOpenGL();
    void suspend();

    virtual void render();
    
    virtual byte* readGameFile(std::string name, unsigned int& size);
    
    virtual void showKeyboard(std::string text);
    virtual void hideKeyboard();

};
