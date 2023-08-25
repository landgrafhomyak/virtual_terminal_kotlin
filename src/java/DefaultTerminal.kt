@file:JvmName("DefaultTerminalKt")

package io.github.landgrafhomyak.virtual_terminal

actual val defaultTerminal: Terminal? = WindowsSystemTerminal.defaultConsole