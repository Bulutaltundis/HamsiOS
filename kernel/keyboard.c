#include "keyboard.h"
#include "shell.h"

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define SHELL_START_ROW 6

static volatile uint16_t* const VGA_MEMORY =
    (volatile uint16_t*)0xB8000;

static uint8_t color = 0x0F;


/* ---------------------------------------------------------
   Terminal cursor
   --------------------------------------------------------- */

static int cursor_row = SHELL_START_ROW;
static int cursor_col = 0;


/* ---------------------------------------------------------
   Command buffer
   --------------------------------------------------------- */

static char command_buffer[128];

static int command_length = 0;


/* ---------------------------------------------------------
   I/O
   --------------------------------------------------------- */

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}


static inline void outb(
    uint16_t port,
    uint8_t value
)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value),
          "Nd"(port)
    );
}


/* ---------------------------------------------------------
   Keyboard map
   --------------------------------------------------------- */

static const char keyboard_map[] =
{
    0,

    27,

    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',

    '-',
    '=',

    '\b',

    '\t',

    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',

    '[',
    ']',

    '\n',

    0,

    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',

    ';',
    '\'',
    '`',

    0,

    '\\',

    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',

    ',',
    '.',
    '/',

    0,

    '*',

    0,

    ' '
};


/* ---------------------------------------------------------
   Terminal
   --------------------------------------------------------- */

static void put_char(char c)
{
    if (cursor_col >= VGA_WIDTH)
    {
        cursor_col = 0;
        cursor_row++;
    }

    if (cursor_row >= VGA_HEIGHT)
    {
        for (int row = SHELL_START_ROW;
             row < VGA_HEIGHT;
             row++)
        {
            for (int col = 0;
                 col < VGA_WIDTH;
                 col++)
            {
                VGA_MEMORY[
                    row * VGA_WIDTH + col
                ] = ((uint16_t)color << 8) | ' ';
            }
        }

        cursor_row = SHELL_START_ROW;
        cursor_col = 0;
    }

    VGA_MEMORY[
        cursor_row * VGA_WIDTH + cursor_col
    ] = ((uint16_t)color << 8) | (uint8_t)c;

    cursor_col++;
}


static void print_prompt(void)
{
    put_char('h');
    put_char('a');
    put_char('m');
    put_char('s');
    put_char('i');
    put_char('>');
    put_char(' ');
}


/* ---------------------------------------------------------
   Execute command
   --------------------------------------------------------- */

static void execute_command(void)
{
    command_buffer[command_length] = '\0';

    cursor_col = 0;
    cursor_row++;

    shell_handle_command(command_buffer);

    command_length = 0;

    print_prompt();
}


/* ---------------------------------------------------------
   Keyboard initialization
   --------------------------------------------------------- */

void keyboard_init(void)
{
    /*
     * Drain keyboard controller output buffer.
     */
    while (inb(0x64) & 1)
    {
        inb(0x60);
    }

    cursor_row = SHELL_START_ROW;
    cursor_col = 0;

    print_prompt();
}


/* ---------------------------------------------------------
   Keyboard interrupt handler
   --------------------------------------------------------- */

void keyboard_handler(void)
{
    uint8_t scancode = inb(0x60);

    /*
     * Key release.
     */
    if (scancode & 0x80)
    {
        outb(0x20, 0x20);
        return;
    }


    if (scancode < sizeof(keyboard_map))
    {
        char c = keyboard_map[scancode];

        if (c)
        {
            /*
             * BACKSPACE
             */
            if (c == '\b')
            {
                if (command_length > 0 &&
                    cursor_col > 7)
                {
                    command_length--;

                    cursor_col--;

                    VGA_MEMORY[
                        cursor_row * VGA_WIDTH +
                        cursor_col
                    ] =
                        ((uint16_t)color << 8) | ' ';
                }
            }

            /*
             * ENTER
             */
            else if (c == '\n')
            {
                execute_command();
            }

            /*
             * NORMAL CHARACTER
             */
            else
            {
                if (command_length < 127)
                {
                    command_buffer[
                        command_length
                    ] = c;

                    command_length++;

                    put_char(c);
                }
            }
        }
    }


    /*
     * End Of Interrupt
     */
    outb(0x20, 0x20);
}
