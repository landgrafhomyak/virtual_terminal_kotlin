package io.github.landgrafhomyak.virtual_terminal

// import java.lang.Object

internal class WindowsSystemTerminal
private constructor(
    private val inHandle: Long,
    private val outHandle: Long
): Terminal, TerminalPollingTask {
    private var isPrepared: Boolean = false
    private var isInAlternateBuffer: Boolean = false
    private var inModeBackup: Long = 0
    private var outModeBackup: Long = 0
    // private var inModeCurrent: Long = 0
    // private var outModeCurrent: Long = 0
    private var isPolling: Boolean = false
    private var stopPollingEventHandle: Long = 0

    override fun prepare() {
        if (this.isPrepared)
            throw IllegalStateException("Terminal already prepared")

        this.isPrepared = true;
        this.inModeBackup = WindowsSystemTerminal._prepareTerminalStdin(this.inHandle)
        this.outModeBackup = WindowsSystemTerminal._prepareTerminalStdout(this.outHandle)
    }

    override fun restore() {
        if (!this.isPrepared)
            throw IllegalStateException("Terminal already restored or not prepared at all")

        this.isPrepared = false;
        WindowsSystemTerminal._restoreTerminalStdin(this.inHandle, this.inModeBackup)
        WindowsSystemTerminal._restoreTerminalStdout(this.outHandle, this.outModeBackup)
    }

    override fun enableWrap(value: Boolean) {}

    override fun enableInsertMode(value: Boolean) {}

    override fun enterAlternateBuffer() {
        if (this.isInAlternateBuffer)
            throw IllegalStateException("Terminal already in alternate buffer")
        this.isInAlternateBuffer = true
        WindowsSystemTerminal._enterAlternateBuffer(this.outHandle)
    }

    override fun leaveAlternateBuffer() {
        if (!this.isInAlternateBuffer)
            throw IllegalStateException("Terminal already not in alternate buffer")
        this.isInAlternateBuffer = false
        WindowsSystemTerminal._leaveAlternateBuffer(this.outHandle)
    }

    private external fun _poll(inHandle: Long, eventHandle: Long, callbacks: TerminalCallbacks)

    override fun poll(callbacks: TerminalCallbacks) {
        if (this.isPolling)
            throw IllegalStateException("Terminal already polling")
        this.isPolling = true
        try {
            this.stopPollingEventHandle = WindowsSystemTerminal._createEvent()
            try {
                callbacks.setPollingTask(this, this)
                this._poll(this.inHandle, this.stopPollingEventHandle, callbacks)
            } finally {
                WindowsSystemTerminal._destroyEvent(this.stopPollingEventHandle)
            }
        } finally {
            this.isPolling = false
        }
    }

    override fun flush() {
        // windows console is unbuffered
    }

    override fun print(chr: Char) {
        WindowsSystemTerminal._print(this.outHandle, chr)
    }

    override fun print(str: String) {
        WindowsSystemTerminal._print(this.outHandle, str)
    }

    override fun moveCursorTo(row: Int, col: Int) {}

    override fun moveCursorToStart() {}

    override fun clear() {
        WindowsSystemTerminal._print(this.outHandle, "\u001b[2J")
    }

    override fun stopTerminalPolling(): Unit {}

    companion object {
        init {
            System.loadLibrary("virtual_terminal_kotlin") // todo
        }
        @JvmStatic
        private external fun _getStdinHandle(): Long
        @JvmStatic
        private external fun _getStdoutHandle(): Long
        @JvmStatic
        var defaultConsole = WindowsSystemTerminal(this._getStdinHandle(), this._getStdoutHandle())

        @JvmStatic
        private external fun _prepareTerminalStdin(inHandle: Long): Long
        @JvmStatic
        private external fun _prepareTerminalStdout(outHandle: Long): Long
        @JvmStatic
        private external fun _restoreTerminalStdin(inHandle: Long, oldState: Long)
        @JvmStatic
        private external fun _restoreTerminalStdout(outHandle: Long, oldState :Long)
        @JvmStatic
        private external fun _enterAlternateBuffer(outHandle: Long)
        @JvmStatic
        private external fun _leaveAlternateBuffer(outHandle: Long)
        @JvmStatic
        private external fun _createEvent(): Long
        @JvmStatic
        private external fun _destroyEvent(eventHandle: Long)
        @JvmStatic
        private external fun _print(outHandle: Long, chr: Char)
        @JvmStatic
        private external fun _print(outHandle: Long, str: String)
    }
}