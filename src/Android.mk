LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := gesturehelper
LOCAL_LDLIBS 	+= -L$(SYSROOT)/usr/lib -llog
LOCAL_SRC_FILES := \
        main.cpp \
		touchutil.cpp

include $(BUILD_EXECUTABLE)
