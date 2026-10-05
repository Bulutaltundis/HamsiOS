#include <stdint.h>

#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "shell.h"


volatile uint16_t* const VGA_MEMORY =
    (uint16_t*)0xB8000;


static uint8_t color = 0x0F;


/* ---------------------------------------------------------
   Clear screen
   --------------------------------------------------------- */

static void clear_screen(void)
{
    for (int i = 0;
         i < 80 * 25;
         i++)
    {
        VGA_MEMORY[i] =
            ((uint16_t)color << 8) | ' ';
    }
}


/* ---------------------------------------------------------
   Print
   --------------------------------------------------------- */

static void print(
    const char* text,
    int row,
    int column
)
{
    int index = row * 80 + column;

    while (*text)
    {
        VGA_MEMORY[index++] =
            ((uint16_t)color << 8) |
            (uint8_t)*text++;

        if (index >= 80 * 25)
            return;
    }
}


/* ---------------------------------------------------------
   Kernel entry
   --------------------------------------------------------- */

void kernel_main(void)
{
    clear_screen();


    /*
     * Boot banner
     */

    print(
        "========================================",
        2,
        20
    );

    print(
        "          HAMSI OS v0.4                 ",
        3,
        20
    );

    print(
        "========================================",
        4,
        20
    );


    /*
     * GDT
     */

    print(
        "[ .. ] Initializing GDT...",
        7,
        20
    );

    gdt_init();

    print(
        "[ OK ] GDT initialized",
        7,
        20
    );


    /*
     * PIC
     */

    print(
        "[ .. ] Initializing PIC...",
        8,
        20
    );

    pic_init();

    print(
        "[ OK ] PIC initialized",
        8,
        20
    );


    /*
     * IDT
     */

    print(
        "[ .. ] Initializing IDT...",
        9,
        20
    );

    idt_init();

    print(
        "[ OK ] IDT initialized",
        9,
        20
    );


    /*
     * Keyboard
     */

    print(
        "[ .. ] Initializing keyboard...",
        10,
        20
    );

    keyboard_init();

    print(
        "[ OK ] Keyboard initialized",
        10,
        20
    );


    /*
     * Shell
     */

    shell_init();


    /*
     * Enable hardware interrupts.
     */

    __asm__ volatile ("sti");


    /*
     * Kernel idle loop.
     */

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}
