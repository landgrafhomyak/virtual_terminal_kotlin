cmake_minimum_required(VERSION 3.25)
project(virtual_terminal_kotlin)

set(CMAKE_CXX_STANDARD 20)

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)

add_library(virtual_terminal_kotlin SHARED src/java/jni_windows.cpp)
target_link_libraries(virtual_terminal_kotlin PRIVATE JNI::JNI)


add_custom_target(
        compile_kotlin_jvm

        COMMAND
        kotlinc-jvm
        -jvm-target ${JVM_TARGET}
        -Xmulti-platform
        -Xcommon-sources=src/common/SystemTerminal.kt;src/common/SystemTerminalCallbacks.kt;src/common/SystemError.kt
        -d classes
        src/common/SystemTerminal.kt
        src/common/SystemTerminalCallbacks.kt
        src/common/SystemError.kt
        src/java/SystemTerminal.kt

        SOURCES
        ./src/common/SystemTerminal.kt
        ./src/common/SystemTerminalCallbacks.kt
        ./src/java/SystemTerminal.kt
        ./src/common/SystemError.kt

        WORKING_DIRECTORY
        ${CMAKE_SOURCE_DIR}
)
add_custom_target(
        generate_jni_headers

        COMMAND
        classh
        -o src/java/jni_functions.h
        -cp classes
        --include-guard-name VIRTUAL_TERMINAL_JNI_H
        --all

        WORKING_DIRECTORY
        ${CMAKE_SOURCE_DIR}

        DEPENDS
        compile_kotlin_jvm
)

