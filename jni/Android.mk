LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)  
LOCAL_MODULE := ffmpeg  
LOCAL_SRC_FILES := libffmpeg.so
include $(PREBUILT_SHARED_LIBRARY) 
 
include $(CLEAR_VARS)
LOCAL_MODULE    := MusicPlay
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES := MusicPlayNative.cpp \
	PlayNative.cpp \
	JniUtil.cpp
LOCAL_SHARED_LIBRARIES := ffmpeg
LOCAL_LDLIBS := -lOpenSLES -llog
include $(BUILD_SHARED_LIBRARY)
