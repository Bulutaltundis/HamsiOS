#include "keyboard.h"
#include "shell.h"
#include "terminal.h"

#include <stdint.h>

static char command_buffer[128];
static int command_length = 0;

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

static const char keyboard_map[] =
{
    0,
    27,

    '1','2','3','4','5','6','7','8','9','0',
    '-','=',
    '\b',
    '\t',

    'q','w','e','r','t','y','u','i','o','p',
    '[',']',

    '\n',
    0,

    'a','s','d','f','g','h','j','k','l',
    ';','\'','`',

    0,
    '\\',

    'z','x','c','v','b','n','m',
    ',','.','/',

    0,
    '*',
    0,
    ' '
};

static void print_prompt(void)
{
    terminal_print("hamsi> ");
}

static void execute_command(void)
{
    command_buffer[command_length] = '\0';

    terminal_newline();

    shell_handle_command(command_buffer);

    command_length = 0;

    print_prompt();
}

void keyboard_init(void)
{
    while (inb(0x64) & 1)
        inb(0x60);

    command_length = 0;

    print_prompt();
}

void keyboard_handler(void)
{
    uint8_t scancode = inb(0x60);

    /*
     * Key release
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
            if (c == '\b')
            {
                if (command_length > 0)
                {
                    command_length--;

                    /*
                     * Terminal'in ortak cursor'ını
                     * bir karakter geri al.
                     */
                    int col = terminal_col();

                    if (col > 0)
                    {
                        /*
                         * Şimdilik görsel olarak
                         * space basıyoruz.
                         */
                        terminal_putchar('\b');
                    }
                }
            }
            else if (c == '\n')
            {
                execute_command();
            }
            else
            {
                if (command_length < 127)
                {
                    command_buffer[command_length++] = c;

                    terminal_putchar(c);
                }
            }
        }
    }

    outb(0x20, 0x20);
}
