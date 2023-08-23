package io.github.landgrafhomyak.virtual_terminal

import kotlin.jvm.JvmStatic

expect class SystemTerminal {
    fun prepareTerminal()

    fun restoreTerminal()

    fun enterAlternateBuffer()

    fun leaveAlternateBuffer()

    class TerminalPollingTask {
        fun stopPolling()
    }

    fun poll(callbacks: SystemTerminalCallbacks): TerminalPollingTask


    fun flush()

    fun print(c: Char)

    fun print(s: String)

    fun moveCursorTo(row: Int, col: Int)

    fun moveCursorToStart()

    fun clear()

    companion object {
        @get:JvmStatic
        val defaultTerminal: SystemTerminal

        @JvmStatic
        fun createTerminal(inFd: Long, outFd: Long, exclusive: Boolean = true): SystemTerminal

        @JvmStatic
        fun pollDefaultTerminal(callbacks: SystemTerminalCallbacks)
    }
}