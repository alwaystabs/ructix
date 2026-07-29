#define UART_BASE 0x10000000

void uart_putchar(char c) {
    volatile char *uart = (char *)UART_BASE;
    *uart = c;
}

void uart_puts(const char *str) {
    while (*str) uart_putchar(*str++);
}