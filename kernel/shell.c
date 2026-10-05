#include "shell.h"
#include "memory.h"

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define SHELL_START_ROW 6

static volatile uint16_t* const VGA =
    (volatile uint16_t*)0xB8000;

static uint8_t color = 0x0F;

static int row = SHELL_START_ROW;
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
        for (int r = SHELL_START_ROW; r < VGA_HEIGHT; r++)
        {
            for (int c = 0; c < VGA_WIDTH; c++)
            {
                VGA[r * VGA_WIDTH + c] =
                    ((uint16_t)color << 8) | ' ';
            }
        }

        row = SHELL_START_ROW;
        col = 0;
    }

    VGA[row * VGA_WIDTH + col] =
        ((uint16_t)color << 8) | (uint8_t)c;

    col++;
}

static void print(const char* text)
{
    while (*text)
    {
        put_char(*text);
        text++;
    }
}

static int string_equals(
    const char* a,
    const char* b
)
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

static int starts_with(
    const char* text,
    const char* prefix
)
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

static void print_number(uint32_t number)
{
    char buffer[16];
    int i = 0;

    if (number == 0)
    {
        put_char('0');
        return;
    }

    while (number > 0)
    {
        buffer[i++] =
            '0' + (number % 10);

        number /= 10;
    }

    while (i > 0)
    {
        put_char(buffer[--i]);
    }
}

static void print_kb(uint32_t bytes)
{
    print_number(bytes / 1024);
    print(" KB");
}

static void command_mem(void)
{
    print("Memory Information\n");
    print("------------------\n");

    print("Heap total : ");
    print_kb(memory_total());
    print("\n");

    print("Heap used  : ");
    print_kb(memory_used());
    print("\n");

    print("Heap free  : ");
    print_kb(memory_free());
    print("\n");
}

static uint32_t parse_number(
    const char* text
)
{
    uint32_t value = 0;

    while (*text >= '0' &&
           *text <= '9')
    {
        value =
            value * 10 +
            (*text - '0');

        text++;
    }

    return value;
}

static void command_alloc(
    const char* command
)
{
    const char* number =
        command + 6;

    if (*number == '\0')
    {
        print("Usage: alloc <bytes>\n");
        return;
    }

    uint32_t size =
        parse_number(number);

    if (size == 0)
    {
        print("Invalid size.\n");
        return;
    }

    void* ptr = kmalloc(size);

    if (!ptr)
    {
        print("Allocation failed.\n");
        return;
    }

    print("Allocated ");
    print_number(size);
    print(" bytes at 0x");

    uint32_t address =
        (uint32_t)ptr;

    char hex[8];
    int i = 0;

    if (address == 0)
    {
        put_char('0');
    }
    else
    {
        while (address && i < 8)
        {
            uint8_t digit =
                address & 0xF;

            hex[i++] =
                digit < 10
                    ? '0' + digit
                    : 'A' + digit - 10;

            address >>= 4;
        }

        while (i > 0)
            put_char(hex[--i]);
    }

    print("\n");
}

static void clear_shell(void)
{
    for (int r = SHELL_START_ROW;
         r < VGA_HEIGHT;
         r++)
    {
        for (int c = 0;
             c < VGA_WIDTH;
             c++)
        {
            VGA[r * VGA_WIDTH + c] =
                ((uint16_t)color << 8) | ' ';
        }
    }

    row = SHELL_START_ROW;
    col = 0;
}

void shell_init(void)
{
    row = SHELL_START_ROW;
    col = 0;

    print("HamsiOS Shell\n");
    print("Type 'help' to see available commands.\n\n");
}

void shell_handle_command(
    const char* command
)
{
    if (string_equals(command, "help"))
    {
        print("Available commands:\n");
        print("  help       Show commands\n");
        print("  clear      Clear terminal\n");
        print("  about      About HamsiOS\n");
        print("  version    Show version\n");
        print("  uname      Show system information\n");
        print("  echo       Print text\n");
        print("  mem        Show memory information\n");
        print("  alloc      Allocate kernel memory\n");
    }
    else if (string_equals(command, "clear"))
    {
        clear_shell();
    }
    else if (string_equals(command, "about"))
    {
        print("HamsiOS\n");
        print("A tiny operating system made by Bulut.\n");
    }
    else if (string_equals(command, "version"))
    {
        print("HamsiOS v0.5\n");
    }
    else if (string_equals(command, "uname"))
    {
        print("HamsiOS i386 kernel\n");
    }
    else if (string_equals(command, "mem"))
    {
        command_mem();
    }
    else if (starts_with(command, "alloc "))
    {
        command_alloc(command);
    }
    else if (starts_with(command, "echo "))
    {
        print(command + 5);
        print("\n");
    }
    else if (*command == '\0')
    {
        return;
    }
    else
    {
        print("Unknown command: ");
        print(command);
        print("\n");
    }
}
