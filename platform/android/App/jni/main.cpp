#include <jni.h>
#include <android/log.h>
#include <errno.h>
#include <math.h>

#include <android_native_app_glue.h>

#include "common.h"
#include "App.h"
#include "AndroidApp.h"
#include "AndroidLogger.h"
#include "input/TouchHandler.h"

AndroidApp* app;

void handle_cmd(android_app* state, int32_t cmd) {
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
        case APP_CMD_RESUME:
            Logger::main->info("Main", "Resume");
            app->active = true;
            break;
        case APP_CMD_PAUSE:
            Logger::main->info("Main", "Pause");
            app->active = false;
            break;
        case APP_CMD_SAVE_STATE:
            state->savedState = null;
            state->savedStateSize = 0;
            break;
    }
}

int32_t handle_input(android_app* state, AInputEvent* event) {
    if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int a = AMotionEvent_getAction(event);
        int action = a & AMOTION_EVENT_ACTION_MASK;
        
        if(action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP || action == AMOTION_EVENT_ACTION_CANCEL) {
            int index = (a & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            int id = AMotionEvent_getPointerId(event, index);
            int x = round(AMotionEvent_getX(event, index));
            int y = round(AMotionEvent_getY(event, index));

            if(action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN) {
                TouchHandler::press(id, x, y);
            } else {
                TouchHandler::release(id, x, y);
            }
        } else if(action == AMOTION_EVENT_ACTION_MOVE) {
            int pCount = AMotionEvent_getPointerCount(event);
            for (int i = 0; i < pCount; i++) {
                int id = AMotionEvent_getPointerId(event, i);
                int x = round(AMotionEvent_getX(event, i));
                int y = round(AMotionEvent_getY(event, i));
                
                TouchHandler::move(id, x, y);
            }
        }
        return 1;
    }
    return 0;
}


void android_main(android_app* state) {
    app_dummy();

    AndroidLogger startupLogger;

    state->onAppCmd = handle_cmd;
    state->onInputEvent = handle_input;

    if(app == null) {
        app = new AndroidApp(state);
        app->init();

        startupLogger.info("Main", "Initialized!");
    }

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
                app->suspend();
                //app->destroyOpenGL();
                delete app;
                app = null;
                return;
            }
        }

        app->render();
    }
}
