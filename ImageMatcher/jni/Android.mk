LOCAL_PATH := $(call my-dir)


#Nonfree module
include $(CLEAR_VARS)
LOCAL_MODULE    := nonfree
LOCAL_SRC_FILES := /home/anupmohan/OpenCV-2.4.8-android-sdk/sdk/native/libs/armeabi-v7a/libnonfree.so
include $(PREBUILT_SHARED_LIBRARY)

#Nonfree module
include $(CLEAR_VARS)
LOCAL_MODULE    := opencv
LOCAL_SRC_FILES := /home/anupmohan/OpenCV-2.4.8-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_java.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

#The following two lines are only needed when statically loading the OpenCV library
OPENCV_CAMERA_MODULES:=off
OPENCV_INSTALL_MODULES:=on

#Change the following line for your correct file path
include /home/anupmohan/OpenCV-2.4.8-android-sdk/sdk/native/jni/OpenCV.mk
LOCAL_MODULE    := imgRetrieval
LOCAL_SRC_FILES := imgRetrieval.cpp
LOCAL_SHARED_LIBRARIES := nonfree opencv
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)
