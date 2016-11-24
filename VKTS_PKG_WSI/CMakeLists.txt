#
# VulKan ToolS window system interface (VKTS WSI) CMake file.
#

cmake_minimum_required(VERSION 3.2)

project (VKTS_PKG_WSI)

set(VKTS_WSI "" CACHE STRING "Enter VKTS_DISPLAY_VISUAL or VKTS_WAYLAND_VISUAL for alternative WSI.")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_External/include ${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_PKG_Core/include  ${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_PKG_Vulkan/include ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

list(APPEND IGNORE_CPP_FILES )

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    find_path(Vulkan_INCLUDE_DIR NAMES vulkan/vulkan.h PATHS "$ENV{VULKAN_SDK}/Include")
    include_directories(AFTER ${Vulkan_INCLUDE_DIR})
    
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        
    endif ()        

    list(APPEND IGNORE_CPP_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_android.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_display.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_xlib.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_wayland.cpp
    )

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    list(APPEND IGNORE_CPP_FILES        ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_android.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_win32.cpp
    )

    if (${VKTS_WSI} MATCHES "VKTS_DISPLAY_VISUAL")

        add_definitions(-DVKTS_DISPLAY_VISUAL)

        list(APPEND IGNORE_CPP_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_xlib.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_wayland.cpp
        )

    elseif (${VKTS_WSI} MATCHES "VKTS_WAYLAND_VISUAL")

        add_definitions(-DVKTS_WAYLAND_VISUAL)

        list(APPEND IGNORE_CPP_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_display.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_xlib.cpp
        )

    else ()

        list(APPEND IGNORE_CPP_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_display.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/window_interface/wsi/fn_wsi_wayland.cpp
        )
        
    endif ()

endif ()

list(REMOVE_ITEM CPP_FILES ${IGNORE_CPP_FILES})

add_library(VKTS_PKG_WSI ${CPP_FILES})

set_property(TARGET VKTS_PKG_WSI PROPERTY CXX_STANDARD 11)
set_property(TARGET VKTS_PKG_WSI PROPERTY CXX_STANDARD_REQUIRED ON)
