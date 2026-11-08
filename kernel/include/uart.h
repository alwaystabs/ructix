#ifndef UART_H
#define UART_H

#define UART_BASE 0x10000000

#ifndef __ASSEMBLER__
void uart_putchar(const char c);
void print(const char *str);
char uart_getchar(void);
#endif

#endif
