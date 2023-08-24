package io.github.landgrafhomyak.virtual_terminal

interface SystemTerminalCallbacks {
    fun setPollingTask(terminal: SystemTerminal, task: SystemTerminal.TerminalPollingTask)
    fun onTerminalResize(terminal: SystemTerminal, newWidth: Short, newHeight: Short)
    fun onTerminalKeyPress(terminal: SystemTerminal, symbol:Char, kbdKeyKode: Byte, alt: Boolean, ctrl: Boolean, shift: Boolean)
}