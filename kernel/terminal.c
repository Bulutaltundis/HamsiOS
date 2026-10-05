#include "terminal.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define TERMINAL_START_ROW 6

static volatile uint16_t* const VGA =
    (volatile uint16_t*)0xB8000;

static uint8_t color = 0x0F;

static int row = TERMINAL_START_ROW;
static int col = 0;

static void scroll(void)
{
    for (int r = TERMINAL_START_ROW;
         r < VGA_HEIGHT - 1;
         r++)
    {
        for (int c = 0;
             c < VGA_WIDTH;
             c++)
        {
            VGA[r * VGA_WIDTH + c] =
                VGA[(r + 1) * VGA_WIDTH + c];
        }
    }

    for (int c = 0;
         c < VGA_WIDTH;
         c++)
    {
        VGA[(VGA_HEIGHT - 1) * VGA_WIDTH + c] =
            ((uint16_t)color << 8) | ' ';
    }

    row = VGA_HEIGHT - 1;
    col = 0;
}

void terminal_init(void)
{
    row = TERMINAL_START_ROW;
    col = 0;
}

void terminal_clear(void)
{
    for (int r = TERMINAL_START_ROW;
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

    row = TERMINAL_START_ROW;
    col = 0;
}

void terminal_newline(void)
{
    col = 0;
    row++;

    if (row >= VGA_HEIGHT)
        scroll();
}

void terminal_putchar(char c)
{
    if (c == '\n')
    {
        terminal_newline();
        return;
    }

    if (c == '\r')
    {
        col = 0;
        return;
    }

    if (col >= VGA_WIDTH)
        terminal_newline();

    VGA[row * VGA_WIDTH + col] =
        ((uint16_t)color << 8) |
        (uint8_t)c;

    col++;

    if (col >= VGA_WIDTH)
        terminal_newline();
}

void terminal_print(const char* text)
{
    while (*text)
    {
        terminal_putchar(*text);
        text++;
    }
}

int terminal_row(void)
{
    return row;
}

int terminal_col(void)
{
    return col;
}
