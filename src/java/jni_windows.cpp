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
