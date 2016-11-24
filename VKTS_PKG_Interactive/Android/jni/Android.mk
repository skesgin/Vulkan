LOCAL_PATH := $(call my-dir)

#
# VKTS.
#
 
include $(CLEAR_VARS)

LOCAL_MODULE := VKTS_PKG_Interactive

# Add all files.

PROJECT_FILES := $(wildcard $(LOCAL_PATH)/../../src/**/**/*.cpp)

# Generate the final source list.

PROJECT_FILES := $(PROJECT_FILES:$(LOCAL_PATH)/%=%)

# Enable C++11.

LOCAL_CPPFLAGS := -std=c++11
LOCAL_CPPFLAGS += -fexceptions

# Includes.

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../VKTS_PKG_Entity/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../VKTS_PKG_Window/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../VKTS_PKG_Runtime/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../VKTS_PKG_Math/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../VKTS_PKG_Core/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../VKTS_PKG_External/include
 
# Sources.

LOCAL_SRC_FILES := $(PROJECT_FILES)

# Libs.
 
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)
