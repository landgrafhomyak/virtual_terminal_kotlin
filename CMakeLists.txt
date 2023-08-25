cmake_minimum_required(VERSION 3.25)
project(virtual_terminal_kotlin)

set(CMAKE_CXX_STANDARD 20)

#find_package(Java REQUIRED)
find_package(JNI REQUIRED)
#include(UseJava)

add_library(virtual_terminal_kotlin SHARED src/java/jni_windows.cpp)
target_link_libraries(virtual_terminal_kotlin PRIVATE JNI::JNI)


include(kotlin.cmake)