#include <cstddef>
#include <cstdint>

#include <Windows.h>

#include "jni_functions.h"
#include "../common/windows_error.hpp"
#include "error_env.hpp"


namespace VirtualTerminalKotlin
{
    template<class ret_t>
    ret_t format_sys_error(JNIEnv *env, ret_t ret, DWORD err = GetLastError()) noexcept
    {
        // if (err == ERROR_SUCCESS)
        //    return false;
        JvmErrorEnv err_env{env};
        if (!err_env.init())
            _format_sys_error<JvmErrorEnv>(&err_env, err);
        return ret;
    }

    void format_sys_error_v(JNIEnv *env, DWORD err = GetLastError()) noexcept
    {
        JvmErrorEnv err_env{env};
        if (!err_env.init())
            _format_sys_error<JvmErrorEnv>(&err_env, err);
    }
}


/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _prepareTerminalStdin
 * Method signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1prepareTerminalStdin__J
        (JNIEnv *env, jclass cls, jlong raw_handle)
{
    HANDLE hIn = (HANDLE) raw_handle;
    DWORD backup_mode;
    if (0 == GetConsoleMode(hIn, &backup_mode))
        return VirtualTerminalKotlin::format_sys_error(env, 0);
    if (0 == SetConsoleMode(hIn, ENABLE_WINDOW_INPUT /*| ENABLE_MOUSE_INPUT*/))
        return VirtualTerminalKotlin::format_sys_error(env, 0);
    return (jlong) backup_mode;
}

/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _prepareTerminalStdout
 * Method signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1prepareTerminalStdout__J
        (JNIEnv *env, jclass cls, jlong raw_handle)
{
    HANDLE hOut = (HANDLE) raw_handle;
    DWORD backup_mode;
    if (0 == GetConsoleMode(hOut, &backup_mode))
        return VirtualTerminalKotlin::format_sys_error(env, 0);
    if (0 == SetConsoleMode(hOut, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        return VirtualTerminalKotlin::format_sys_error(env, 0);
    return (jlong) backup_mode;
}

/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _restoreTerminalStdin
 * Method signature: (JJ)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1restoreTerminalStdin__JJ
        (JNIEnv *env, jclass cls, jlong raw_handle, jlong backup_mode)
{
    HANDLE hIn = (HANDLE) raw_handle;
    if (0 == SetConsoleMode(hIn, (DWORD) backup_mode))
        return VirtualTerminalKotlin::format_sys_error_v(env);
}

/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _restoreTerminalStdout
 * Method signature: (JJ)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1restoreTerminalStdout__JJ
        (JNIEnv *env, jclass cls, jlong raw_handle, jlong backup_mode)
{
    HANDLE hOut = (HANDLE) raw_handle;
    if (0 == SetConsoleMode(hOut, (DWORD) backup_mode))
        return VirtualTerminalKotlin::format_sys_error_v(env);
}

/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _enterAlternateBuffer
 * Method signature: (J)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1enterAlternateBuffer__J
        (JNIEnv *env, jclass cls, jlong raw_handle)
{
    HANDLE hOut = (HANDLE) raw_handle;
    if (0 == WriteConsoleW(hOut, "\\x1b[?1049h", 8, nullptr, nullptr))
        return VirtualTerminalKotlin::format_sys_error_v(env);
}
/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _leaveAlternateBuffer
 * Method signature: (J)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1leaveAlternateBuffer__J
        (JNIEnv *env, jclass cls, jlong raw_handle)
{
    HANDLE hOut = (HANDLE) raw_handle;
    if (0 == WriteConsoleW(hOut, "\\x1b[?1049l", 8, nullptr, nullptr))
        return VirtualTerminalKotlin::format_sys_error_v(env);
}


/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _createEvent
 * Method signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1createEvent__
        (JNIEnv *env, jclass cls)
{
    HANDLE hEvent = CreateEventA(nullptr, true, false, nullptr);
    if (hEvent == nullptr)
        return VirtualTerminalKotlin::format_sys_error(env, 0);
    return (jlong) hEvent;
}
/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _destroyEvent
 * Method signature: (J)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1destroyEvent__J
        (JNIEnv *env, jclass cls, jlong raw_handle)
{
    if (0 == CloseHandle((HANDLE) raw_handle))
        return VirtualTerminalKotlin::format_sys_error_v(env);
}


/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _print
 * Method signature: (JC)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1print__JC
        (JNIEnv *env, jclass cls, jlong raw_handle, jchar chr)
{
    HANDLE hOut = (HANDLE) raw_handle;
    WCHAR str[1] = {(WCHAR) chr};
    if (0 == WriteConsoleW(hOut, str, 1, nullptr, nullptr))
        return VirtualTerminalKotlin::format_sys_error_v(env);
}

/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _print
 * Method signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1print__JLjava_lang_String_2
        (JNIEnv *env, jclass cls, jlong raw_handle, jstring str_o)
{
    HANDLE hOut = (HANDLE) raw_handle;

    jboolean is_copy;
    WCHAR const *str;
    static_assert(sizeof(WCHAR) == sizeof(jchar), "Can't implicitly convert java char to wchar");
    str = (WCHAR const *) env->GetStringChars(str_o, &is_copy);
    if (str == nullptr)
        return;

    if (0 == WriteConsoleW(hOut, str, env->GetStringLength(str_o), nullptr, nullptr))
        return VirtualTerminalKotlin::format_sys_error_v(env);
}

static_assert(sizeof(HANDLE) <= sizeof(jlong), "Can't hold WinApi HANDLE as java long");

/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _getStdinHandle
 * Method signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1getStdinHandle__
        (JNIEnv *env, jclass cls)
{
    jclass ioexception_cls = env->FindClass("java/io/IOException");
    if (ioexception_cls == nullptr)
        return 0;

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn == nullptr)
    {
        env->ThrowNew(ioexception_cls, "Standard input stream not available");
        return 0;
    }
    else if (hIn == INVALID_HANDLE_VALUE)
    {
        return VirtualTerminalKotlin::format_sys_error(env, 0);
    }
    return (jlong) hIn;
}

/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _getStdoutHandle
 * Method signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1getStdoutHandle__
        (JNIEnv *env, jclass cls)
{
    jclass ioexception_cls = env->FindClass("java/io/IOException");
    if (ioexception_cls == nullptr)
        return 0;

    HANDLE hIn = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hIn == nullptr)
    {
        env->ThrowNew(ioexception_cls, "Standard output stream not available");
        return 0;
    }
    else if (hIn == INVALID_HANDLE_VALUE)
    {
        return VirtualTerminalKotlin::format_sys_error(env, 0);
    }
    return (jlong) hIn;
}



/*
 * Class: io.github.landgrafhomyak.virtual_terminal.WindowsSystemTerminal
 * Method name: _poll
 * Method signature: (JJLio/github/landgrafhomyak/virtual_terminal/TerminalCallbacks;)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_WindowsSystemTerminal__1poll__JJLio_github_landgrafhomyak_virtual_1terminal_TerminalCallbacks_2
        (JNIEnv *env, jobject self, jlong raw_in_handle, jlong raw_event_handle, jobject _callbacks_o)
{
    HANDLE hIn = (HANDLE) raw_in_handle;
    HANDLE hEvent = (HANDLE) raw_event_handle;
    struct
    {
        jobject instance;
        jclass cls;
        jmethodID resize_method;
        jmethodID key_press_method;
    } callbacks;

    callbacks.instance = _callbacks_o;
    callbacks.cls = env->FindClass("io/github/landgrafhomyak/virtual_terminal/TerminalCallbacks");
    if (callbacks.cls == nullptr)
        return;
    callbacks.resize_method = env->GetMethodID(callbacks.cls, "onTerminalResize", "(Lio/github/landgrafhomyak/virtual_terminal/Terminal;SS)V");
    if (callbacks.resize_method == nullptr)
        return;
    callbacks.key_press_method = env->GetMethodID(callbacks.cls, "onTerminalKeyPress", "(Lio/github/landgrafhomyak/virtual_terminal/Terminal;CBZZZ)V");
    if (callbacks.key_press_method == nullptr)
        return;

    HANDLE wait_objects[2] = {hEvent, hIn}; // in this order
    while (true)
    {
        CONTINUE_LOOP:
        switch (WaitForMultipleObjects(2, wait_objects, false, INFINITE))
        {
            case WAIT_OBJECT_0:
                return;
            case WAIT_OBJECT_0 + 1:
            {
                static constexpr DWORD MAX_INPUTS_COUNT = 8;
                INPUT_RECORD inputs[MAX_INPUTS_COUNT];
                DWORD inputs_count;
                if (0 == ReadConsoleInputW(hIn, inputs, MAX_INPUTS_COUNT, &inputs_count))
                {
                    return VirtualTerminalKotlin::format_sys_error_v(env);
                }
                for (DWORD i = 0; i < inputs_count; i++)
                {
                    switch (inputs[i].EventType)
                    {
                        case WINDOW_BUFFER_SIZE_EVENT:
                            env->CallVoidMethod(callbacks.instance, callbacks.resize_method, self, inputs[i].Event.WindowBufferSizeEvent.dwSize.X, inputs[i].Event.WindowBufferSizeEvent.dwSize.Y);
                            if (env->ExceptionCheck() == JNI_TRUE)
                                return;

                        case KEY_EVENT:;
                            if (inputs[i].Event.KeyEvent.bKeyDown)
                            {
                                env->CallVoidMethod(
                                        callbacks.instance,
                                        callbacks.key_press_method,
                                        self,
                                        (jchar) inputs[i].Event.KeyEvent.uChar.UnicodeChar,
                                        (jbyte) inputs[i].Event.KeyEvent.wVirtualKeyCode,
                                        (inputs[i].Event.KeyEvent.dwControlKeyState & LEFT_ALT_PRESSED) != 0 || (inputs[i].Event.KeyEvent.dwControlKeyState & RIGHT_ALT_PRESSED) != 0 ? JNI_TRUE : JNI_FALSE,
                                        (inputs[i].Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED) != 0 || (inputs[i].Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED) != 0 ? JNI_TRUE : JNI_FALSE,
                                        (inputs[i].Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED) != 0 ? JNI_TRUE : JNI_FALSE
                                );
                                if (env->ExceptionCheck() == JNI_TRUE)
                                    return;
                            }
                    }
                }
                break;
            }
            case WAIT_FAILED:
                return VirtualTerminalKotlin::format_sys_error_v(env);
        }
    }
}