#include "JNIHelper.h"

#include "common.h"
#include "utils/Logger.h"
#include "input/KeyboardHandler.h"

JNIHelper::JNIHelper(android_app * app) {
    this->app = app;
    app->activity->vm->AttachCurrentThread(&env, null);

    object = app->activity->clazz;
    clazz = env->GetObjectClass(object);

    showKeyboardID = env->GetMethodID(clazz, "showKeyboard", "(Ljava/lang/String;)V");
    hideKeyboardID = env->GetMethodID(clazz, "hideKeyboard", "()V");
}

JNIHelper::~JNIHelper() {
    app->activity->vm->DetachCurrentThread();
}

void JNIHelper::showKeyboard(std::string currentText) {
    jstring s = env->NewStringUTF(currentText.c_str());
    env->CallVoidMethod(object, showKeyboardID, s);
}

void JNIHelper::hideKeyboard() {
    env->CallVoidMethod(object, hideKeyboardID);
}

extern "C" {

JNIEXPORT void JNICALL Java_net_lbsg_client_MainActivity_onTextUpdated(JNIEnv* env, jclass clazz, jstring newText) {
    const char* str = env->GetStringUTFChars(newText, 0);
    Logger::main->trace("JNIHelper", "Text updated: %s", str);
    KeyboardHandler::inputSetText(std::string(str));
    env->ReleaseStringUTFChars(newText, str);
}

}
