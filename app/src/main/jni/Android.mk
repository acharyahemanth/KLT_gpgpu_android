MY_LOCAL_PATH           := $(call my-dir)
LOCAL_PATH              := $(MY_LOCAL_PATH)

## external paths
OPENCV_PATH = /Users/hemanth/Code/work/OpenCV-3.0.0-android-sdk/sdk/native/jni
GLM_PATH = /Users/hemanth/Code/work/glm

## main library
include $(CLEAR_VARS)
OPENCV_INSTALL_MODULES:=on
OPENCV_CAMERA_MODULES:=off
OPENCV_LIB_TYPE:=STATIC
include $(OPENCV_PATH)/OpenCV.mk
LOCAL_MODULE            := KLTGPGPUNative
LOCAL_SRC_FILES         := jni_apis.cc \
                           KLT_gpu_engine.cpp \
                           standalone_test_engine.cpp
LOCAL_LDLIBS            += -lm -llog -landroid -lEGL -lz -lGLESv3
LOCAL_STATIC_LIBRARIES  += klt_gpgpu ndk_helper android_native_app_glue
LOCAL_CPP_EXTENSION     := .cxx .cpp .cc
LOCAL_C_INCLUDES        += $(GLM_PATH)
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)
include $(BUILD_SHARED_LIBRARY)

## KLT_gpgpu
include $(MY_LOCAL_PATH)/KLT_gpgpu/Android.mk

$(call import-module,android/ndk_helper)
$(call import-module,android/native_app_glue)
