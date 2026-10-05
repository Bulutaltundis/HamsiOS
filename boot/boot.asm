bits 32

section .multiboot
align 4

multiboot_header:
    dd 0xE85250D6
    dd 0
    dd multiboot_header_end - multiboot_header
    dd -(0xE85250D6 + 0 + (multiboot_header_end - multiboot_header))

    dw 0
    dw 0
    dd 8

multiboot_header_end:

section .text

global _start
extern kernel_main

_start:
    cli

    mov esp, stack_top

    call kernel_main

.hang:
    hlt
    jmp .hang

section .bss

align 16

stack_bottom:
    resb 16384
stack_top:
