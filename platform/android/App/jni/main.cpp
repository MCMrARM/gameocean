#include <jni.h>
#include <android/log.h>
#include <errno.h>

#include <android_native_app_glue.h>

#include "common.h"
#include "App.h"
#include "AndroidApp.h"
#include "AndroidLogger.h"

void handle_cmd(android_app* state, int32_t cmd) {
    AndroidApp* app = (AndroidApp*) state->userData;
    if(app == null) {
        return;
    }

    switch(cmd) {
        case APP_CMD_INIT_WINDOW:
            Logger::main->info("Main", "Initialize window");
            if(state->window != null) {
                app->initOpenGL();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            Logger::main->info("Main", "Terminate window");
            //app->destroyOpenGL();
            app->suspend();
            app->active = false;
            break;
        case APP_CMD_GAINED_FOCUS:
            Logger::main->info("Main", "Gained focus");
            app->active = true;
            break;
        case APP_CMD_LOST_FOCUS:
            Logger::main->info("Main", "Lost focus");
            app->active = false;
            break;
        case APP_CMD_SAVE_STATE:
            state->savedState = null;
            state->savedStateSize = 0;
            break;
    }
}


void android_main(android_app* state) {
    app_dummy();

    AndroidLogger startupLogger;

    state->onAppCmd = handle_cmd;

    AndroidApp* app = new AndroidApp(state);
    app->init();

    state->userData = app;

    startupLogger.info("Main", "Initialized!");

    android_poll_source* source;
    int eId;
    int events;
    while(true) {
        source = null;
        while((eId = ALooper_pollAll(app->active ? 0 : -1, null, &events, (void**) &source)) >= 0) {
            if(source != null) {
                source->process(state, source);
            }

            if(state->destroyRequested != 0) {
                Logger::main->info("Main", "Destroy requested");
                app->destroyOpenGL();
                return;
            }
        }

        app->render();
    }

}
