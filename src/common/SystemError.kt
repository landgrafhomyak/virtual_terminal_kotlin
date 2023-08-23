package io.github.landgrafhomyak.virtual_terminal

import kotlin.jvm.JvmStatic

class SystemError: RuntimeException {
    val errno: Int
    val rawMessage: String?

    private companion object {
        @JvmStatic
        private fun formatMessage(errno: Int, message: String?): String {
            if (message == null)
                return "[System error ${errno}]"
            else
                return "[System error ${errno}] ${message}"
        }
    }

    constructor(errno: Int, message: String?): super(SystemError.formatMessage(errno, message)) {
         this.errno = errno
         this.rawMessage = message
    }

    constructor(errno: Int, message: String?, cause: Throwable): super(SystemError.formatMessage(errno, message), cause) {
        this.errno = errno
        this.rawMessage = message
    }
}
