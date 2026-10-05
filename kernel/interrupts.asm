bits 32

section .text

global keyboard_interrupt
extern keyboard_handler

keyboard_interrupt:
    pusha

    call keyboard_handler

    popa

    iretd
