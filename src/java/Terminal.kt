package io.github.landgrafhomyak.virtual_terminal

import java.io.IOException

actual interface Terminal {
    /*********/
    @Throws(IOException::class)
    actual fun prepare()

    @Throws(IOException::class)
    actual fun restore()

    @Throws(IOException::class)
    actual fun enableWrap(value: Boolean)

    @Throws(IOException::class)
    actual fun enableInsertMode(value: Boolean)

    /*************/
    @Throws(IOException::class)
    actual fun enterAlternateBuffer()

    @Throws(IOException::class)
    actual fun leaveAlternateBuffer()

    @Throws(IOException::class)
    actual fun poll(callbacks: TerminalCallbacks)

    @Throws(IOException::class)
    actual fun flush()

    @Throws(IOException::class)
    actual fun print(chr: Char)

    @Throws(IOException::class)
    actual fun print(str: String)

    @Throws(IOException::class)
    actual fun moveCursorTo(row: Int, col: Int)

    @Throws(IOException::class)
    actual fun moveCursorToStart()

    @Throws(IOException::class)
    actual fun clear()
}