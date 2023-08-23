#ifndef VirtualTerminalKotlin_JVM_ERROR_ENV_HPP
# define VirtualTerminalKotlin_JVM_ERROR_ENV_HPP

# include <jni.h>


namespace VirtualTerminalKotlin
{
    class JvmErrorEnv
    {
    private:
        JNIEnv *env;

        struct
        {
            jclass cls;
            jmethodID set_cause;
            jmethodID add_suppressed;
        } throwable;
        struct
        {
            jclass cls;
            jmethodID constructor;
        } system_error;
    public:
        explicit JvmErrorEnv(JNIEnv *env) noexcept: env{env}
        {}

        bool init() noexcept
        {
            this->throwable.cls = env->FindClass("java/lang/Throwable");
            if (this->throwable.cls == nullptr)
                return true;

            this->throwable.set_cause = env->GetMethodID(this->throwable.cls, "initCause", "(Ljava/lang/Throwable;)Ljava/lang/Throwable;");
            if (this->throwable.set_cause == nullptr)
                return true;

            this->throwable.add_suppressed = env->GetMethodID(this->throwable.cls, "addSuppressed", "(Ljava/lang/Throwable;)V");
            if (this->throwable.add_suppressed == nullptr)
                return true;

            this->throwable.cls = env->FindClass("java/lang/Throwable");
            if (this->throwable.cls == nullptr)
                return true;

            this->throwable.set_cause = env->GetMethodID(this->throwable.cls, "initCause", "(Ljava/lang/Throwable;)Ljava/lang/Throwable;");
            if (this->throwable.set_cause == nullptr)
                return true;

            this->throwable.add_suppressed = env->GetMethodID(this->throwable.cls, "addSuppressed", "(Ljava/lang/Throwable;)V");
            if (this->throwable.add_suppressed == nullptr)
                return true;

            this->system_error.cls = env->FindClass("io/github/landgrafhomyak/virtual_terminal/SystemError");
            if (this->system_error.cls == nullptr)
                return true;

            this->system_error.constructor = env->GetMethodID(this->system_error.cls, "<init>", "(ILjava/lang/String;)V");
            if (this->system_error.constructor == nullptr)
                return true;

            return false;
        }

        using T_NULLABLE_STRING = jstring;
        using T_EXCEPTION = jthrowable;
        using T_NULLABLE_EXCEPTION = jthrowable;

        T_NULLABLE_STRING new_string(char *const utf8) noexcept
        {
            return this->env->NewStringUTF(utf8);
        }

        static constexpr jstring O_NULLABLE_STRING_ERR = nullptr;
        static constexpr jstring O_NULLABLE_STRING_NULL = nullptr;

        void dec_ref(jstring ref) noexcept
        {
            this->env->DeleteLocalRef(ref);
        }


        T_EXCEPTION new_sys_exception(jint err, T_NULLABLE_STRING message) noexcept
        {
            return (jthrowable) env->NewObject(this->system_error.cls, this->system_error.constructor, err, message);
        }

        static constexpr jthrowable O_EXCEPTION_ERR = nullptr;
        static constexpr jthrowable O_NULLABLE_EXCEPTION_NULL = nullptr;

        void dec_ref(jthrowable ref) noexcept
        {
            this->env->DeleteLocalRef(ref);
        }

        void throw_(jthrowable err) noexcept
        {
            this->env->Throw(err);
        }

        bool set_cause(jthrowable err, jthrowable cause) noexcept
        {
            env->CallObjectMethod(err, this->throwable.set_cause, cause);
            if (env->ExceptionCheck() == JNI_TRUE)
                return true;
            return false;

        }

        bool set_suppressed(jthrowable err, jthrowable suppressed) noexcept
        {
            env->CallVoidMethod(err, this->throwable.add_suppressed, suppressed);
            if (env->ExceptionCheck() == JNI_TRUE)
                return true;
            return false;
        }
    };
}

#endif // VirtualTerminalKotlin_JVM_ERROR_ENV_HPP
