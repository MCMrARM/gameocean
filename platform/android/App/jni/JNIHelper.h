#pragma once

#include <android_native_app_glue.h>
#include <jni.h>
#include <string>

class JNIHelper {

private:
    android_app* app;
    JNIEnv* env;
    jclass clazz;
    jobject object;
    jmethodID showKeyboardID;
    jmethodID hideKeyboardID;

public:
    JNIHelper(android_app *app);
    ~JNIHelper();

    void showKeyboard(std::string currentText);
    void hideKeyboard();

};
