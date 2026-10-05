#ifndef TERMINAL_H
#define TERMINAL_H

void terminal_init(void);
void terminal_clear(void);

void terminal_putchar(char c);
void terminal_print(const char* text);
void terminal_newline(void);

void terminal_backspace(void);
void terminal_delete(void);

int terminal_row(void);
int terminal_col(void);
void terminal_set_cursor(int row, int col);

#endif
