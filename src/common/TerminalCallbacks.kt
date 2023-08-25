package io.github.landgrafhomyak.virtual_terminal

interface TerminalCallbacks {
    fun setPollingTask(terminal: Terminal, task: TerminalPollingTask)
    fun onTerminalResize(terminal: Terminal, newWidth: Short, newHeight: Short)
    fun onTerminalKeyPress(terminal: Terminal, symbol:Char, kbdKeyKode: Byte, alt: Boolean, ctrl: Boolean, shift: Boolean)
}