cmake_minimum_required(VERSION 2.8)

project(realtimereplacement)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -framework GLUT -framework OpenGL") # compiler version and compiler flags.

set(SOURCE_FILES Sinwave3D/main.cpp) # which files should be compiled

include_directories("/Users/benspecht/CLionProjects/untitled8/glm")

add_executable(realtimereplacement ${SOURCE_FILES}) # which files should produce executables

target_link_libraries(realtimereplacement ${SDL2_LIBRARY})
