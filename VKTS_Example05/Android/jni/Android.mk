LOCAL_PATH			:= $(call my-dir)

#
# VKTS.
#

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_VulkanGui
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_VulkanGui/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_VulkanGui.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_Gui
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_Gui/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_Gui.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_VulkanScenegraph
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_VulkanScenegraph/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_VulkanScenegraph.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_Scenegraph
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_Scenegraph/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_Scenegraph.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_VulkanComposition
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_VulkanComposition/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_VulkanComposition.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_VulkanWindow
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_VulkanWindow/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_VulkanWindow.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_VulkanWrapper
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_VulkanWrapper/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_VulkanWrapper.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_Interactive
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_Interactive/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_Interactive.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_Window
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_Window/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_Window.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_Entity
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_Entity/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_Entity.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_Image
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_Image/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_Image.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_Runtime
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_Runtime/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_Runtime.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_Math
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_Math/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_Math.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VKTS_PKG_Core
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../VKTS_PKG_Core/Android/obj/local/$(TARGET_ARCH_ABI)/libVKTS_PKG_Core.a
include $(PREBUILT_STATIC_LIBRARY)

#
# Example.
#

include $(CLEAR_VARS)

LOCAL_MODULE := VKTS_Example05

# All files.

PROJECT_FILES := $(wildcard $(LOCAL_PATH)/../../src/*.cpp)

# Generate the final list.

PROJECT_FILES := $(PROJECT_FILES:$(LOCAL_PATH)/%=%)

# Enable C++11.

LOCAL_CPPFLAGS := -std=c++11
LOCAL_CPPFLAGS += -fexceptions

# Includes.

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../VKTS/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../VKTS_External/include

# Sources.

LOCAL_SRC_FILES := $(PROJECT_FILES)

# Libs.

LOCAL_LDLIBS    := -landroid -lvulkan

LOCAL_STATIC_LIBRARIES := VKTS_PKG_VulkanGui
LOCAL_STATIC_LIBRARIES += VKTS_PKG_Gui
LOCAL_STATIC_LIBRARIES += VKTS_PKG_VulkanScenegraph
LOCAL_STATIC_LIBRARIES += VKTS_PKG_Scenegraph
LOCAL_STATIC_LIBRARIES += VKTS_PKG_VulkanComposition
LOCAL_STATIC_LIBRARIES += VKTS_PKG_VulkanWindow
LOCAL_STATIC_LIBRARIES += VKTS_PKG_VulkanWrapper
LOCAL_STATIC_LIBRARIES += VKTS_PKG_Interactive
LOCAL_STATIC_LIBRARIES += VKTS_PKG_Entity
LOCAL_STATIC_LIBRARIES += VKTS_PKG_Image
LOCAL_STATIC_LIBRARIES += VKTS_PKG_Runtime
LOCAL_STATIC_LIBRARIES += VKTS_PKG_Math
LOCAL_STATIC_LIBRARIES += VKTS_PKG_Core
LOCAL_STATIC_LIBRARIES += VKTS_PKG_Window
LOCAL_STATIC_LIBRARIES += android_native_app_glue
LOCAL_STATIC_LIBRARIES += cpufeatures

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,android/cpufeatures)
