package io.github.landgrafhomyak.virtual_terminal

import java.lang.System

@Suppress("INAPPLICABLE_JVM_NAME")
actual class SystemTerminal {
    private val inFd: Long
    private val outFd: Long
    private var isPrepared: Boolean = false
    private var isInAlternateBuffer: Boolean = false
    private var inBackupState: Long = 0
    private var outBackupState: Long = 0

    private constructor(inFd: Long, outFd: Long) {
        this.inFd = inFd
        this.outFd = outFd
    }

    actual fun prepareTerminal() {
        if (this.isPrepared)
            throw IllegalStateException("Terminal already prepared")

        this.isPrepared = true;
        this.inBackupState = SystemTerminal._prepareTerminalStdin(this.inFd)
        this.outBackupState = SystemTerminal._prepareTerminalStdout(this.outFd)
    }

    actual fun restoreTerminal() {
        if (!this.isPrepared)
            throw IllegalStateException("Terminal already restored or not prepared at all")

        this.isPrepared = false;
        SystemTerminal._restoreTerminalStdin(this.inFd, this.inBackupState)
        SystemTerminal._restoreTerminalStdout(this.outFd, this.outBackupState)
    }

    actual fun enterAlternateBuffer() {
        if (this.isInAlternateBuffer)
            throw IllegalStateException("Terminal already in alternate buffer")
        this.isInAlternateBuffer = true
        SystemTerminal._print(this.outFd, "\u001b[?1049h")
    }

    actual fun leaveAlternateBuffer() {
        if (!this.isInAlternateBuffer)
            throw IllegalStateException("Terminal already not in alternate buffer")
        this.isInAlternateBuffer = false
        SystemTerminal._print(this.outFd ,"\u001b[?1049l")
    }

    actual class TerminalPollingTask private constructor(private val eventHandle: Long){
        private var isIntercepted: Boolean = false
        private var isInterceptedWithException: Boolean = false
        actual fun stopPolling(withException:Boolean): Unit = TODO()
    }

    actual fun poll(callbacks: SystemTerminalCallbacks) {
        this._poll(this.inFd, callbacks)
    }

    private external fun _poll(inFd: Long, callbacks: SystemTerminalCallbacks)

    actual fun flush() {
        SystemTerminal._flush(this.outFd)
    }

    actual fun print(c: Char) {
        SystemTerminal._print(this.outFd, c)
    }

    actual fun print(s: String) {
        SystemTerminal._print(this.outFd, s)
    }

    actual fun moveCursorTo(row: Int, col: Int) {}

    actual fun moveCursorToStart() {}

    actual fun clear() {
        SystemTerminal._print(this.outFd, "\u001b[2J")
    }

    actual companion object {
        init {
            System.loadLibrary("virtual_terminal_kotlin")
        }

        private var _defaultTerminal: SystemTerminal? = null

        @get:JvmStatic
        actual val defaultTerminal: SystemTerminal
            get() = this._defaultTerminal ?: SystemTerminal(this._getStdinFd(), this._getStdoutFd()).also { t -> this._defaultTerminal = t }

        @JvmStatic
        actual fun createTerminal(inFd: Long, outFd: Long, exclusive: Boolean): SystemTerminal = TODO()

        @JvmStatic
        actual fun pollDefaultTerminal(callbacks: SystemTerminalCallbacks) {
            val t = this.defaultTerminal
            t.prepareTerminal()
            try {
                t.enterAlternateBuffer()
                try {
                    t.clear()
                    t.flush()
                    t.poll(callbacks)
                } finally {
                    t.leaveAlternateBuffer()
                }
            } finally {
                t.restoreTerminal()
            }
        }



        @JvmStatic
        private external fun _prepareTerminalStdin(inFd: Long): Long
        @JvmStatic
        private external fun _prepareTerminalStdout(outFd: Long): Long
        @JvmStatic
        private external fun _restoreTerminalStdin(inFd: Long, oldState: Long)
        @JvmStatic
        private external fun _restoreTerminalStdout(outFd: Long, oldState :Long)
        @JvmStatic
        private external fun _flush(outFd: Long)
        @JvmStatic
        private external fun _print(outFd: Long, c: Char)
        @JvmStatic
        private external fun _print(outFd: Long, s: String)
        @JvmStatic
        private external fun _getStdinFd(): Long
        @JvmStatic
        private external fun _getStdoutFd(): Long
    }
}