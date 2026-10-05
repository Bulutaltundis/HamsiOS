#include "idt.h"

struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idtp;

extern void keyboard_interrupt(void);

static void idt_set_gate(
    int number,
    uint32_t handler,
    uint16_t selector,
    uint8_t flags
)
{
    idt[number].offset_low = handler & 0xFFFF;
    idt[number].selector = selector;
    idt[number].zero = 0;
    idt[number].type_attr = flags;
    idt[number].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_init(void)
{
    for (int i = 0; i < 256; i++)
    {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
        idt[i].offset_high = 0;
    }

    idt_set_gate(
        33,
        (uint32_t)keyboard_interrupt,
        0x08,
        0x8E
    );

    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;

    __asm__ volatile ("lidt %0" : : "m"(idtp));
}
