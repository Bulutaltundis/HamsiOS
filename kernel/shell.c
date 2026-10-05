#include "shell.h"
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static volatile uint16_t* const VGA =
    (volatile uint16_t*)0xB8000;

static uint8_t color = 0x0F;

static int row = 6;
static int col = 0;

static void put_char(char c)
{
    if (c == '\n')
    {
        col = 0;
        row++;
        return;
    }

    if (col >= VGA_WIDTH)
    {
        col = 0;
        row++;
    }

    if (row >= VGA_HEIGHT)
    {
        for (int r = 6; r < VGA_HEIGHT; r++)
        {
            for (int c2 = 0; c2 < VGA_WIDTH; c2++)
            {
                VGA[r * VGA_WIDTH + c2] =
                    ((uint16_t)color << 8) | ' ';
            }
        }

        row = 6;
        col = 0;
    }

    VGA[row * VGA_WIDTH + col] =
        ((uint16_t)color << 8) | c;

    col++;
}

static void print(const char* text)
{
    while (*text)
        put_char(*text++);
}

static int equals(const char* a, const char* b)
{
    while (*a && *b)
    {
        if (*a != *b)
            return 0;

        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

static int starts_with(const char* text, const char* prefix)
{
    while (*prefix)
    {
        if (*text != *prefix)
            return 0;

        text++;
        prefix++;
    }

    return 1;
}

static void clear_screen(void)
{
    for (int r = 6; r < VGA_HEIGHT; r++)
    {
        for (int c = 0; c < VGA_WIDTH; c++)
        {
            VGA[r * VGA_WIDTH + c] =
                ((uint16_t)color << 8) | ' ';
        }
    }

    row = 6;
    col = 0;
}

void shell_init(void)
{
    row = 6;
    col = 0;

    print("HamsiOS Shell\n");
    print("Type 'help' to see available commands.\n\n");
}

void shell_handle_command(const char* command)
{
    if (equals(command, "help"))
    {
        print("Available commands:\n");
        print("  help       Show commands\n");
        print("  clear      Clear terminal\n");
        print("  about      About HamsiOS\n");
        print("  version    Show version\n");
        print("  uname      Show system information\n");
        print("  echo       Print text\n");
    }
    else if (equals(command, "clear"))
    {
        clear_screen();
    }
    else if (equals(command, "about"))
    {
        print("HamsiOS\n");
        print("A tiny operating system made by Bulut.\n");
    }
    else if (equals(command, "version"))
    {
        print("HamsiOS v0.4\n");
    }
    else if (equals(command, "uname"))
    {
        print("HamsiOS i386 kernel\n");
    }
    else if (starts_with(command, "echo "))
    {
        print(command + 5);
        print("\n");
    }
    else if (*command)
    {
        print("Unknown command: ");
        print(command);
        print("\n");
    }
}
