#include "multiboot.h"
#include "terminal.h"

static uint32_t total_memory = 0;
static uint32_t multiboot_address = 0;

static void print_hex64(uint64_t value)
{
    char buffer[16];
    int i = 0;

    if (value == 0)
    {
        terminal_putchar('0');
        return;
    }

    while (value && i < 16)
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

static void print_memory_type(uint32_t type)
{
    switch (type)
    {
        case 1:
            terminal_print("Available");
            break;

        case 2:
            terminal_print("Reserved ");
            break;

        case 3:
            terminal_print("ACPI     ");
            break;

        case 4:
            terminal_print("NVS      ");
            break;

        case 5:
            terminal_print("Bad RAM  ");
            break;

        default:
            terminal_print("Unknown  ");
            break;
    }
}

void multiboot_parse(uint32_t address)
{
    multiboot_address = address;
    total_memory = 0;

    struct multiboot_tag* tag =
        (struct multiboot_tag*)
        (address + 8);

    while (tag->type != MULTIBOOT_TAG_TYPE_END)
    {
        if (tag->type == MULTIBOOT_TAG_TYPE_MMAP)
        {
            struct multiboot_tag_mmap* mmap =
                (struct multiboot_tag_mmap*)tag;

            uint32_t offset = 0;

            while (
                offset <
                mmap->size -
                sizeof(struct multiboot_tag_mmap)
            )
            {
                struct multiboot_mmap_entry* entry =
                    (struct multiboot_mmap_entry*)
                    (
                        (uint8_t*)mmap +
                        sizeof(struct multiboot_tag_mmap) +
                        offset
                    );

                if (entry->type ==
                    MULTIBOOT_MEMORY_AVAILABLE)
                {
                    uint64_t end =
                        entry->addr +
                        entry->len;

                    if (end > total_memory)
                    {
                        if (end > 0xFFFFFFFF)
                            total_memory = 0xFFFFFFFF;
                        else
                            total_memory =
                                (uint32_t)end;
                    }
                }

                offset += mmap->entry_size;
            }
        }

        tag =
            (struct multiboot_tag*)
            (
                (uint8_t*)tag +
                ((tag->size + 7) & ~7)
            );
    }
}

uint32_t multiboot_total_memory(void)
{
    return total_memory;
}

void multiboot_print_memory_map(void)
{
    struct multiboot_tag* tag =
        (struct multiboot_tag*)
        (multiboot_address + 8);

    terminal_print("Physical Memory Map\n");
    terminal_print("===================\n");

    while (tag->type != MULTIBOOT_TAG_TYPE_END)
    {
        if (tag->type == MULTIBOOT_TAG_TYPE_MMAP)
        {
            struct multiboot_tag_mmap* mmap =
                (struct multiboot_tag_mmap*)tag;

            uint32_t offset = 0;

            while (
                offset <
                mmap->size -
                sizeof(struct multiboot_tag_mmap)
            )
            {
                struct multiboot_mmap_entry* entry =
                    (struct multiboot_mmap_entry*)
                    (
                        (uint8_t*)mmap +
                        sizeof(struct multiboot_tag_mmap) +
                        offset
                    );

                terminal_print("0x");
                print_hex64(entry->addr);

                terminal_print(" - 0x");

                print_hex64(
                    entry->addr +
                    entry->len
                );

                terminal_print("  ");

                print_memory_type(entry->type);

                terminal_print("\n");

                offset += mmap->entry_size;
            }
        }

        tag =
            (struct multiboot_tag*)
            (
                (uint8_t*)tag +
                ((tag->size + 7) & ~7)
            );
    }
}
