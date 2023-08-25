set(
        KOTLIN_COMMON_SOURCES

        ./src/common/DefaultTerminal.kt
        ./src/common/SystemError.kt
        ./src/common/TerminalCallbacks.kt
        ./src/common/Terminal.kt
        ./src/common/TerminalPollingTask.kt
        ./src/common/Wrappers.kt
)

set(
        KOTLIN_JVM_SOURCES

        ./src/java/DefaultTerminal.kt
#        ./src/java/SystemTerminal.kt
        ./src/java/Terminal.kt
        ./src/java/WindowsSystemTerminal.kt
)



add_custom_target(
        compile_kotlin_jvm

        COMMAND
        kotlinc-jvm
        -jvm-target ${JVM_TARGET}
        -Xmulti-platform
        -Xcommon-sources="${KOTLIN_COMMON_SOURCES}" # todo $<list:join>
        -d classes
        ${KOTLIN_COMMON_SOURCES}
        ${KOTLIN_JVM_SOURCES}

        SOURCES
        ${KOTLIN_COMMON_SOURCES}
        ${KOTLIN_JVM_SOURCES}

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
