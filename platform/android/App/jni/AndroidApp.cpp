#include "AndroidApp.h"

#include <android/asset_manager.h>

#include "utils/Logger.h"
#include "AndroidLogger.h"

AndroidApp::AndroidApp(android_app* state) : App() {
    this->androidApp = state;
}

MultiLogger* AndroidApp::initLogger() {
	MultiLogger* logger = App::initLogger();
	logger->addLogger(new AndroidLogger());
	return logger;
}

void AndroidApp::initOpenGL() {
    Logger::main->trace("AndroidApp", "Initializing EGL context...");


    if(display == EGL_NO_DISPLAY) {
        const EGLint attribs[] = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_DEPTH_SIZE, 16,
                EGL_NONE
        };

        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        eglInitialize(display, 0, 0);

        EGLint numConfigs;
        eglChooseConfig(display, attribs, &eglConfig, 1, &numConfigs);

        EGLint format;
        eglGetConfigAttrib(display, eglConfig, EGL_NATIVE_VISUAL_ID, &format);
        Logger::main->debug("AndroidApp", "EGL Format: %i", format);
        ANativeWindow_setBuffersGeometry(androidApp->window, 0, 0, format);
    }

    surface = eglCreateWindowSurface(display, eglConfig, androidApp->window, NULL);

    const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    if(context == EGL_NO_CONTEXT) {
        context = eglCreateContext(display, eglConfig, NULL, contextAttribs);
    }

    if(eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        Logger::main->error("AndroidApp", "Unable to eglMakeCurrent");

        int err = eglGetError();
        if(err == EGL_CONTEXT_LOST) {
            context = eglCreateContext(display, eglConfig, NULL, contextAttribs);
            if(eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
                return;
            } else {
                this->destroyOpenGL();
                this->initOpenGL();
            }
        }

        return;
    }

    int w, h;
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
    
    this->resize(w, h);

    Logger::main->trace("AndroidApp", "EGL has been initialized!");

    App::initOpenGL();
}

void AndroidApp::suspend() {
    Logger::main->trace("AndroidApp", "Destroying EGL surface...");

    if(surface != EGL_NO_SURFACE) {
        eglDestroySurface(display, surface);
        surface = EGL_NO_SURFACE;
    }
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

byte* AndroidApp::readGameFile(std::string name, unsigned int& size) {
    AAssetManager* assets = androidApp->activity->assetManager;
    AAsset* asset = AAssetManager_open(assets, name.c_str(), AASSET_MODE_UNKNOWN);
    if(asset == null) {
        Logger::main->error("AndroidApp", "Couldn't read asset: %s", name.c_str());
        size = 0;
        return null;
    }
    long s = AAsset_getLength(asset);
    if(s >= UINT_MAX) {
        Logger::main->error("AndroidApp", "Asset %s is too big [%d]", name.c_str(), s);
        return null;
    }
    Logger::main->trace("AndroidApp", "Reading asset: %s, size: %i", name.c_str(), s);
    size = (unsigned int) s;
    byte* arr = (byte*) malloc(size);
    AAsset_read(asset, arr, (size_t) size);
    AAsset_close(asset);

    return arr;
}