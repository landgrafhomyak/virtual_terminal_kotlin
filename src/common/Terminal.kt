package io.github.landgrafhomyak.virtual_terminal

expect interface Terminal {
    /*************/
    fun prepare()

    fun restore()

    fun enableWrap(value: Boolean)

    fun enableInsertMode(value: Boolean)

    /*************/
    fun enterAlternateBuffer()

    fun leaveAlternateBuffer()

    fun poll(callbacks: TerminalCallbacks)

    fun flush()

    fun print(chr: Char)

    fun print(str: String)

    fun moveCursorTo(row: Int, col: Int)

    fun moveCursorToStart()

    fun clear()
}