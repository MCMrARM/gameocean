LOCAL_PATH := $(call my-dir)
CWD := $(shell pwd)

include $(CLEAR_VARS)

LOCAL_MODULE := client
LOCAL_CPPFLAGS := -std=c++11 -fexceptions -Wno-error=format-security
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lz
LOCAL_STATIC_LIBRARIES := android_native_app_glue

GAME_SOURCES := $(CWD)/../../../src
LIBS_SOURCES := $(CWD)/../../../libs

LOCAL_SRC_FILES := $(shell find $(CWD)/jni -name '*.cpp') $(shell find $(LIBS_SOURCES) -name '*.c') $(shell find $(GAME_SOURCES) -name '*.cpp')

LOCAL_C_INCLUDES += $(GAME_SOURCES) $(LIBS_SOURCES)

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)