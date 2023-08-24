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
 * Class: io.github.landgrafhomyak.virtual_terminal.SystemTerminal
 * Method name: prepareTerminalStdin
 * Method signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_SystemTerminal__1prepareTerminalStdin__J
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
 * Class: io.github.landgrafhomyak.virtual_terminal.SystemTerminal
 * Method name: prepareTerminalStdout
 * Method signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_SystemTerminal__1prepareTerminalStdout__J
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
 * Class: io.github.landgrafhomyak.virtual_terminal.SystemTerminal
 * Method name: restoreTerminalStdin
 * Method signature: (JJ)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_SystemTerminal__1restoreTerminalStdin__JJ
        (JNIEnv *env, jclass cls, jlong raw_handle, jlong backup_mode)
{
    HANDLE hIn = (HANDLE) raw_handle;
    if (0 == SetConsoleMode(hIn, (DWORD) backup_mode))
        return VirtualTerminalKotlin::format_sys_error_v(env);
}

/*
 * Class: io.github.landgrafhomyak.virtual_terminal.SystemTerminal
 * Method name: restoreTerminalStdout
 * Method signature: (JJ)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_SystemTerminal__1restoreTerminalStdout__JJ
        (JNIEnv *env, jclass cls, jlong raw_handle, jlong backup_mode)
{
    HANDLE hOut = (HANDLE) raw_handle;
    if (0 == SetConsoleMode(hOut, (DWORD) backup_mode))
        return VirtualTerminalKotlin::format_sys_error_v(env);
}

/*
 * Class: io.github.landgrafhomyak.virtual_terminal.SystemTerminal
 * Method name: flush0
 * Method signature: (J)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_SystemTerminal__1flush__J
        (JNIEnv *env, jclass cls, jlong raw_handle)
{
    // windows stdout is unbuffered
# if 0
    HANDLE hOut = (HANDLE) raw_handle;
    if (0 == FlushFileBuffers(hOut))
        return VirtualTerminalKotlin::format_sys_error_v(env);
# endif
}

/*
 * Class: io.github.landgrafhomyak.virtual_terminal.SystemTerminal
 * Method name: print0
 * Method signature: (JC)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_SystemTerminal__1print__JC
        (JNIEnv *env, jclass cls, jlong raw_handle, jchar chr)
{
    HANDLE hOut = (HANDLE) raw_handle;
    WCHAR str[1] = {(WCHAR) chr};
    if (0 == WriteConsoleW(hOut, str, 1, nullptr, nullptr))
        return VirtualTerminalKotlin::format_sys_error_v(env);
}

/*
 * Class: io.github.landgrafhomyak.virtual_terminal.SystemTerminal
 * Method name: print0
 * Method signature: (JLjava.lang.String;)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_SystemTerminal__1print__JLjava_lang_String_2
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
 * Class: io.github.landgrafhomyak.virtual_terminal.SystemTerminal
 * Method name: getStdinFd
 * Method signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_SystemTerminal__1getStdinFd__
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
 * Class: io.github.landgrafhomyak.virtual_terminal.SystemTerminal
 * Method name: getStdoutFd
 * Method signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_SystemTerminal__1getStdoutFd__
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
 * Class: io.github.landgrafhomyak.virtual_terminal.SystemTerminal
 * Method name: _poll
 * Method signature: (JLio.github.landgrafhomyak.virtual/terminal.SystemTerminalCallbacks;)V
 */
JNIEXPORT void JNICALL
Java_io_github_landgrafhomyak_virtual_1terminal_SystemTerminal__1poll__JLio_github_landgrafhomyak_virtual_1terminal_SystemTerminalCallbacks_2
        (JNIEnv *env, jobject self, jlong raw_handle, jobject _callbacks_o)
{
    HANDLE hIn = (HANDLE) raw_handle;
    struct
    {
        jclass cls;
        jmethodID constructor;
        jfieldID is_intercepted_field;
        jfieldID interception_with_exception_field;
        jobject instance;
    } polling_task;
    struct
    {
        jobject instance;
        jclass cls;
        jmethodID set_task_method;
        jmethodID resize_method;
        jmethodID key_press_method;
    } callbacks;

    /**********************************************************/

    polling_task.cls = env->FindClass("io/github/landgrafhomyak/virtual_terminal/SystemTerminal$TerminalPollingTask");
    if (polling_task.cls == nullptr)
        return;
    polling_task.constructor = env->GetMethodID(polling_task.cls, "<init>", "(J)V");
    if (polling_task.constructor == nullptr)
        return;
    polling_task.is_intercepted_field = env->GetFieldID(polling_task.cls, "isIntercepted", "Z");
    if (polling_task.is_intercepted_field == nullptr)
        return;
    polling_task.interception_with_exception_field = env->GetFieldID(polling_task.cls, "isInterceptedWithException", "Z");
    if (polling_task.interception_with_exception_field == nullptr)
        return;


    callbacks.instance = _callbacks_o;
    callbacks.cls = env->FindClass("io/github/landgrafhomyak/virtual_terminal/SystemTerminalCallbacks");
    if (callbacks.cls == nullptr)
        return;
    callbacks.set_task_method = env->GetMethodID(callbacks.cls, "setPollingTask", "(Lio/github/landgrafhomyak/virtual_terminal/SystemTerminal;Lio/github/landgrafhomyak/virtual_terminal/SystemTerminal$TerminalPollingTask;)V");
    if (callbacks.set_task_method == nullptr)
        return;
    callbacks.resize_method = env->GetMethodID(callbacks.cls, "onTerminalResize", "(Lio/github/landgrafhomyak/virtual_terminal/SystemTerminal;SS)V");
    if (callbacks.set_task_method == nullptr)
        return;
    callbacks.key_press_method = env->GetMethodID(callbacks.cls, "onTerminalKeyPress", "(Lio/github/landgrafhomyak/virtual_terminal/SystemTerminal;CBZZZ)V");
    if (callbacks.key_press_method == nullptr)
        return;

    /**********************************************************/

    HANDLE hEvent = CreateEventA(nullptr, true, false, nullptr);
    if (hEvent == nullptr)
    {
        return VirtualTerminalKotlin::format_sys_error_v(env);
    }
    else
    {
        polling_task.instance = env->NewObject(polling_task.cls, polling_task.constructor, (jlong) hEvent);
        if (polling_task.instance == nullptr)
            goto CLOSE_hEvent_AND_RETURN;

        env->CallVoidMethod(callbacks.instance, callbacks.set_task_method, self, polling_task.instance);
        if (env->ExceptionCheck() == JNI_TRUE)
            goto CLOSE_hEvent_AND_RETURN;


        HANDLE wait_objects[2] = {hEvent, hIn}; // in this order
        while (true)
        {
            CONTINUE_LOOP:
            switch (WaitForMultipleObjects(2, wait_objects, false, INFINITE))
            {
                case WAIT_OBJECT_0:
                    goto STOP_POLLING;
                case WAIT_OBJECT_0 + 1:
                {
                    static constexpr DWORD MAX_INPUTS_COUNT = 8;
                    INPUT_RECORD inputs[MAX_INPUTS_COUNT];
                    DWORD inputs_count;
                    if (0 == ReadConsoleInputW(hIn, inputs, MAX_INPUTS_COUNT, &inputs_count))
                    {
                        VirtualTerminalKotlin::format_sys_error_v(env);
                        goto CLOSE_hEvent_AND_RETURN;
                    }
                    for (DWORD i = 0; i < inputs_count; i++)
                    {
                        switch (inputs[i].EventType)
                        {
                            case WINDOW_BUFFER_SIZE_EVENT:
                                env->CallVoidMethod(callbacks.instance, callbacks.resize_method, self, inputs[i].Event.WindowBufferSizeEvent.dwSize.X, inputs[i].Event.WindowBufferSizeEvent.dwSize.Y);
                                if (env->ExceptionCheck() == JNI_TRUE)
                                    goto CLOSE_hEvent_AND_RETURN;

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
                                        goto CLOSE_hEvent_AND_RETURN;
                                }
                        }
                    }
                    break;
                }
                case WAIT_FAILED:
                    VirtualTerminalKotlin::format_sys_error_v(env);
                    goto CLOSE_hEvent_AND_RETURN;
            }
        }
    }
    CLOSE_hEvent_AND_RETURN:
    CloseHandle(hEvent);
    return;

    STOP_POLLING:
    goto CLOSE_hEvent_AND_RETURN;

}