#
# VulKan ToolS Core (VKTS_PKG_Core) CMake file.
#

cmake_minimum_required(VERSION 3.2)

project (VKTS_PKG_Core)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_External/include ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

list(APPEND IGNORE_CPP_FILES )

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    find_path(Vulkan_INCLUDE_DIR NAMES vulkan/vulkan.h PATHS "$ENV{VULKAN_SDK}/Include")
    include_directories(AFTER ${Vulkan_INCLUDE_DIR})
    
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        
    endif ()        

    list(APPEND IGNORE_CPP_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/processor/fn_processor_android.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/processor/fn_processor_linux.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/profile/fn_profile_general.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/file/fn_file_android.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/file/fn_file_linux.cpp                                    
    )

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    list(APPEND IGNORE_CPP_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/processor/fn_processor_android.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/processor/fn_processor_windows.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/profile/fn_profile_windows.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/file/fn_file_android.cpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/file/fn_file_windows.cpp
    )

endif ()

list(REMOVE_ITEM CPP_FILES ${IGNORE_CPP_FILES})

add_library(VKTS_PKG_Core ${CPP_FILES})

set_property(TARGET VKTS_PKG_Core PROPERTY CXX_STANDARD 11)
set_property(TARGET VKTS_PKG_Core PROPERTY CXX_STANDARD_REQUIRED ON)
