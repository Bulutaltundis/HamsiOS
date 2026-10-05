#include "shell.h"
#include "memory.h"
#include "terminal.h"
#include "multiboot.h"
#include "frame.h"

#include <stdint.h>

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
        terminal_putchar('0');
        return;
    }

    while (number > 0)
    {
        buffer[i++] =
            '0' + (number % 10);

        number /= 10;
    }

    while (i > 0)
        terminal_putchar(buffer[--i]);
}

static void print_kb(uint32_t bytes)
{
    print_number(bytes / 1024);
    terminal_print(" KB");
}

static void command_mem(void)
{
    terminal_print("Memory Information\n");
    terminal_print("------------------\n");

    terminal_print("Heap total : ");
    print_kb(memory_total());
    terminal_print("\n");

    terminal_print("Heap used  : ");
    print_kb(memory_used());
    terminal_print("\n");

    terminal_print("Heap free  : ");
    print_kb(memory_free());
    terminal_print("\n");
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

static void print_hex(uint32_t value)
{
    char buffer[8];
    int i = 0;

    if (value == 0)
    {
        terminal_putchar('0');
        return;
    }

    while (value && i < 8)
    {
        uint8_t digit = value & 0xF;

        if (digit < 10)
            buffer[i++] = '0' + digit;
        else
            buffer[i++] = 'A' + digit - 10;

        value >>= 4;
    }

    while (i > 0)
        terminal_putchar(buffer[--i]);
}

static void command_alloc(
    const char* command
)
{
    const char* number =
        command + 6;

    if (*number == '\0')
    {
        terminal_print("Usage: alloc <bytes>\n");
        return;
    }

    uint32_t size =
        parse_number(number);

    if (size == 0)
    {
        terminal_print("Invalid size.\n");
        return;
    }

    void* ptr = kmalloc(size);

    if (!ptr)
    {
        terminal_print("Allocation failed.\n");
        return;
    }

    terminal_print("Allocated ");
    print_number(size);
    terminal_print(" bytes at 0x");

    print_hex((uint32_t)ptr);

    terminal_print("\n");
}

void shell_init(void)
{
    terminal_print("HamsiOS Shell\n");
    terminal_print(
        "Type 'help' to see available commands.\n\n"
    );
}

void shell_handle_command(
    const char* command
)
{
    if (string_equals(command, "help"))
    {
        terminal_print("Available commands:\n");
        terminal_print("  help       Show commands\n");
        terminal_print("  clear      Clear terminal\n");
        terminal_print("  about      About HamsiOS\n");
        terminal_print("  version    Show version\n");
        terminal_print("  uname      Show system information\n");
        terminal_print("  echo       Print text\n");
        terminal_print("  mem        Show memory information\n");
        terminal_print("  memmap     Show physical memory map\n");
        terminal_print("  frames     Show physical frame information\n");
        terminal_print("  framealloc Allocate one physical frame\n");
        terminal_print("  alloc      Allocate kernel memory\n");
    }
    else if (string_equals(command, "clear"))
    {
        terminal_clear();
    }
    else if (string_equals(command, "about"))
    {
        terminal_print("HamsiOS\n");
        terminal_print(
            "A tiny operating system made by Bulut.\n"
        );
    }
    else if (string_equals(command, "version"))
    {
        terminal_print("HamsiOS v0.7\n");
    }
    else if (string_equals(command, "uname"))
    {
        terminal_print("HamsiOS i386 kernel\n");
    }
    else if (string_equals(command, "mem"))
    {
        command_mem();
    }
    else if (string_equals(command, "memmap"))
    {
        multiboot_print_memory_map();
    }
    else if (string_equals(command, "frames"))
    {
        terminal_print("Physical Frames\n");
        terminal_print("----------------\n");

        terminal_print("Frame size : ");
        print_number(FRAME_SIZE);
        terminal_print(" bytes\n");

        terminal_print("Total      : ");
        print_number(frame_total());
        terminal_print("\n");

        terminal_print("Used       : ");
        print_number(frame_used());
        terminal_print("\n");

        terminal_print("Free       : ");
        print_number(frame_free_count());
        terminal_print("\n");
    }
    else if (string_equals(command, "framealloc"))
    {
        uint32_t address = frame_alloc();

        if (address == 0)
        {
            terminal_print("No free frames.\n");
            return;
        }

        terminal_print("Allocated frame: 0x");

        print_hex(address);

        terminal_print("\n");
    }
    else if (starts_with(command, "alloc "))
    {
        command_alloc(command);
    }
    else if (starts_with(command, "echo "))
    {
        terminal_print(command + 5);
        terminal_print("\n");
    }
    else if (*command == '\0')
    {
        return;
    }
    else
    {
        terminal_print("Unknown command: ");
        terminal_print(command);
        terminal_print("\n");
    }
}
