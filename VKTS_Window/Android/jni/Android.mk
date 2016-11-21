LOCAL_PATH := $(call my-dir)

#
# VKTS.
#
 
include $(CLEAR_VARS)

LOCAL_MODULE := VKTS_Window

# Add all files.

PROJECT_FILES := $(wildcard $(LOCAL_PATH)/../../src/**/**/*.cpp)

# Create list of operating system specific files and remove them.
IGNORE_FILES := $(LOCAL_PATH)/../../src/window/visual/fn_visual_linux_gamepad.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/window/visual/fn_visual_linux_touchpad.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/window/visual/fn_visual_xlib.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/window/visual/fn_visual_xlib_key.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/window/visual/fn_visual_win32.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/window/visual/fn_visual_win32_key.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/window/visual/fn_visual_display.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/window/visual/fn_visual_display_linux.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/window/visual/fn_visual_display_linux_key.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/window/visual/fn_visual_wayland.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/window/visual/fn_visual_wayland_test.cpp
IGNORE_FILES += $(LOCAL_PATH)/../../src/window/visual/fn_visual_wayland_key.cpp

PROJECT_FILES := $(filter-out $(IGNORE_FILES), $(PROJECT_FILES))

# Generate the final source list.

PROJECT_FILES := $(PROJECT_FILES:$(LOCAL_PATH)/%=%)

# Enable C++11.

LOCAL_CPPFLAGS := -std=c++11
LOCAL_CPPFLAGS += -fexceptions

# Includes.

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../VKTS_Core/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../VKTS_External/include
 
# Sources.

LOCAL_SRC_FILES := $(PROJECT_FILES)

# Libs.
 
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)
