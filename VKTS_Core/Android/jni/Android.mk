LOCAL_PATH := $(call my-dir)

#
# VKTS.
#
 
include $(CLEAR_VARS)

LOCAL_MODULE := VKTS_Core

# Add all files.

PROJECT_FILES := $(wildcard $(LOCAL_PATH)/../../src/**/**/*.cpp)

# Create list of operating system specific files and remove them.

IGNORE_FILES := $(LOCAL_PATH)/../../src/core/processor/fn_processor_linux.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/core/processor/fn_processor_windows.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/core/profile/fn_profile_windows.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/core/file/fn_file_general.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/core/file/fn_file_linux.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/core/file/fn_file_windows.cpp

PROJECT_FILES := $(filter-out $(IGNORE_FILES), $(PROJECT_FILES))

# Generate the final source list.

PROJECT_FILES := $(PROJECT_FILES:$(LOCAL_PATH)/%=%)

# Enable C++11.

LOCAL_CPPFLAGS := -std=c++11
LOCAL_CPPFLAGS += -fexceptions

# Includes.

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../VKTS_External/include
 
# Sources.

LOCAL_SRC_FILES := $(PROJECT_FILES)

# Libs.
 
LOCAL_STATIC_LIBRARIES := android_native_app_glue
LOCAL_STATIC_LIBRARIES += cpufeatures

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,android/cpufeatures)
