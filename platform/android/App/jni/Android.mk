LOCAL_PATH := $(call my-dir)
CWD := $(shell pwd)

include $(CLEAR_VARS)

LOCAL_MODULE := client
LOCAL_CPPFLAGS := -std=c++11 -fexceptions -Wno-error=format-security
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lz
LOCAL_STATIC_LIBRARIES := android_native_app_glue

GAME_SOURCES := $(CWD)/../../../src
COMMON_SOURCES := $(GAME_SOURCES)/common
CLIENT_SOURCES := $(GAME_SOURCES)/client
LIBPNG_SOURCES := $(CWD)/../../../libs/png
LIBJSON_SOURCES := $(CWD)/../../../libs/json
LIBGLM_SOURCES := $(CWD)/../../../libs/glm

LOCAL_SRC_FILES := $(shell find $(CWD)/jni -name '*.cpp') $(shell find $(LIBPNG_SOURCES) -name '*.c') $(shell find $(LIBJSON_SOURCES) -name '*.cpp') $(shell find $(COMMON_SOURCES) -name '*.cpp') $(shell find $(CLIENT_SOURCES) -name '*.cpp')

LOCAL_C_INCLUDES += $(COMMON_SOURCES) $(CLIENT_SOURCES) $(LIBPNG_SOURCES) $(LIBJSON_SOURCES) $(LIBGLM_SOURCES)

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
