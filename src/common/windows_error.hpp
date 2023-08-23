#ifndef VirtualTerminalKotlin_WINDOWS_ERROR_HPP
# define VirtualTerminalKotlin_WINDOWS_ERROR_HPP

# ifndef _WIN32
#  error "This header only for Windows systems"
# endif

# include <Windows.h>

namespace VirtualTerminalKotlin
{
    namespace
    {
        template<class env_t, unsigned format_message_levels = 2>
        void _format_sys_error0(env_t *env, DWORD err_code = GetLastError(), typename env_t::T_NULLABLE_EXCEPTION suppressed = env_t::O_NULLABLE_EXCEPTION_NULL) noexcept
        {
            char *msg_raw = nullptr;
            DWORD fmt_err;
            if constexpr (format_message_levels > 0)
            {
                fmt_err = FormatMessageA(
                        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                        nullptr,
                        err_code,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        (LPTSTR) &msg_raw,
                        0,
                        nullptr
                );

                if (fmt_err == 0)
                    fmt_err = GetLastError();
                else
                    fmt_err = ERROR_SUCCESS;
            }

            typename env_t::T_NULLABLE_STRING msg_o;
            if constexpr (format_message_levels > 0)
            {
                if (msg_raw != nullptr)
                {

                    size_t pos = lstrlenA(msg_raw);
                    while (pos-- > 0)
                    {
                        switch (msg_raw[pos])
                        {
                            case '\n':
                            case '\r':
                                msg_raw[pos] = '\0';
                            default:
                                break;
                        }
                    }
                    msg_o = env->new_string(msg_raw);
                    LocalFree(msg_raw);
                    if (msg_o == env_t::O_NULLABLE_STRING_ERR)
                        return;
                }
                else
                {
                    msg_o = env_t::O_NULLABLE_STRING_NULL;
                }
            }
            else
            {
                msg_o = env_t::O_NULLABLE_STRING_NULL;
            }


            typename env_t::T_EXCEPTION err2throw;
            typename env_t::T_EXCEPTION sys_err = err2throw = env->new_sys_exception(err_code, msg_o);
            if (msg_o != nullptr) env->dec_ref(msg_o);
            if (sys_err == env_t::O_EXCEPTION_ERR)
            {
                return;
            }
            if (suppressed != env_t::O_NULLABLE_EXCEPTION_NULL)
            {
                if (env->set_suppressed(err2throw, suppressed))
                {
                    return;
                }
            }

            // todo resolve codes

            if constexpr (format_message_levels > 0)
            {
                if (fmt_err != ERROR_SUCCESS)
                {
                    _format_sys_error0<env_t, format_message_levels - 1>(env, fmt_err, err2throw);
                }
                else
                {
                    env->throw_(err2throw);
                }
            }
            else
            {
                env->throw_(err2throw);
            }

            env->dec_ref(err2throw);
        }
    }

    template<class env_t>
    void _format_sys_error(env_t *env, DWORD err) noexcept
    {
        return _format_sys_error0<env_t>(env, err);
    }
}

#endif // VirtualTerminalKotlin_WINDOWS_ERROR_HPP
