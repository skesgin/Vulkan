#
# VulKan ToolS Math (VKTS_PKG_Math) CMake file.
#

cmake_minimum_required(VERSION 3.2)

project (VKTS_PKG_Math)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_External/include ${CMAKE_CURRENT_SOURCE_DIR}/../VKTS/include)

file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

if (${ANDROID_PLATFORM} MATCHES "android")

    include_directories(AFTER $ENV{ANDROID_NDK_HOME}/sources/android/native_app_glue)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    find_path(Vulkan_INCLUDE_DIR NAMES vulkan/vulkan.h PATHS "$ENV{VULKAN_SDK}/Include")
    include_directories(AFTER ${Vulkan_INCLUDE_DIR})
    
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        
    endif ()        

endif ()

add_library(VKTS_PKG_Math ${CPP_FILES})

set_property(TARGET VKTS_PKG_Math PROPERTY CXX_STANDARD 11)
set_property(TARGET VKTS_PKG_Math PROPERTY CXX_STANDARD_REQUIRED ON)
