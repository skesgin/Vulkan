#
# VulKan ToolS (VKTS) CMake file.
#

cmake_minimum_required(VERSION 3.2)

project (VKTS_PKG_Window)

set(VKTS_WSI "" CACHE STRING "Enter VKTS_DISPLAY_VISUAL or VKTS_WAYLAND_VISUAL for alternative WSI.")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_External/include ${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_PKG_Core/include ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

list(APPEND IGNORE_CPP_FILES )

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    find_path(Vulkan_INCLUDE_DIR NAMES vulkan/vulkan.h PATHS "$ENV{VULKAN_SDK}/Include")
    include_directories(AFTER ${Vulkan_INCLUDE_DIR})
    
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        
    endif ()        

    list(APPEND IGNORE_CPP_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_android.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_android_key.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_display.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_display_linux.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_display_linux_key.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_linux_gamepad.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_linux_touchpad.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_xlib.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_xlib_key.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_wayland.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_wayland_test.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_wayland_key.cpp
    )

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    list(APPEND IGNORE_CPP_FILES        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_android.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_android_key.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_win32.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_win32_key.cpp
    )

    if (${VKTS_WSI} MATCHES "VKTS_DISPLAY_VISUAL")

        add_definitions(-DVKTS_DISPLAY_VISUAL)

        list(APPEND IGNORE_CPP_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_xlib.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_xlib_key.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_wayland.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_wayland_test.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_wayland_key.cpp
        )

    elseif (${VKTS_WSI} MATCHES "VKTS_WAYLAND_VISUAL")

        add_definitions(-DVKTS_WAYLAND_VISUAL)

        list(APPEND IGNORE_CPP_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_display.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_display_linux.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_display_linux_key.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_linux_touchpad.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_wayland_test.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_xlib.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_xlib_key.cpp
        )

    else ()

        list(APPEND IGNORE_CPP_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_display.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_display_linux.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_display_linux_key.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_wayland.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_wayland_test.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window/visual/fn_visual_wayland_key.cpp
        )
        
    endif ()

endif ()

list(REMOVE_ITEM CPP_FILES ${IGNORE_CPP_FILES})

add_library(VKTS_PKG_Window ${CPP_FILES})

set_property(TARGET VKTS_PKG_Window PROPERTY CXX_STANDARD 11)
set_property(TARGET VKTS_PKG_Window PROPERTY CXX_STANDARD_REQUIRED ON)
