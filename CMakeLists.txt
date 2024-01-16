cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(pygeode VERSION 1.0.0)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

target_include_directories(${PROJECT_NAME} PRIVATE src/)

setup_geode_mod(${PROJECT_NAME})

set(Python_EXECUTABLE "${CMAKE_SOURCE_DIR}/py/python.exe")
set(Python_LIBRARIES "${CMAKE_SOURCE_DIR}/py/python312.lib")
set(Python_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/py/include")
set(Python_ADDITIONAL_VERSIONS 3)

# find_package(Python REQUIRED COMPONENTS Development)

# target_link_options(${PROJECT_NAME} PRIVATE /DELAYLOAD:python312.dll)
target_include_directories(${PROJECT_NAME} PRIVATE ${Python_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Python_LIBRARIES} delayimp.lib)
