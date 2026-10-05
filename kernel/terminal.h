#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

void terminal_init(void);

void terminal_clear(void);

void terminal_putchar(char c);
void terminal_print(const char* text);
void terminal_newline(void);

int terminal_row(void);
int terminal_col(void);

#endif
