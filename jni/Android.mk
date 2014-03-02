LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := sift_prebuilt
LOCAL_SRC_FILES := libnonfree.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := opencv_java_prebuilt
LOCAL_SRC_FILES := libopencv_java.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

OPENCV_CAMERA_MODULES:=off

include /home/rohit/android/OpenCV-2.4.3.2-android-sdk/sdk/native/jni/OpenCV.mk	
LOCAL_C_INCLUDES:= /home/rohit/android/OpenCV-2.4.3.2-android-sdk/sdk/native/jni/include
LOCAL_MODULE    := vision
LOCAL_SRC_FILES := vision.cpp
LOCAL_CFLAGS    := -O3 -ffast-math
LOCAL_LDLIBS 	+=  -llog -ldl
LOCAL_SHARED_LIBRARIES := sift_prebuilt opencv_java_prebuilt
include $(BUILD_SHARED_LIBRARY)
