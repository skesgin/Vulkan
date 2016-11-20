#
# VulKan ToolS Math (VKTS_Math) CMake file.
#

cmake_minimum_required(VERSION 3.2)

project (VKTS_Math)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_External/include ${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_Core/include ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(VKTS_Math ${CPP_FILES})

set_property(TARGET VKTS_Math PROPERTY CXX_STANDARD 11)
set_property(TARGET VKTS_Math PROPERTY CXX_STANDARD_REQUIRED ON)
