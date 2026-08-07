#define UART_BASE 0x10000000

void uart_putchar(const char c) {
    volatile char *uart = (char *)UART_BASE;
    *uart = c;
}

void print(const char *str) {
    while (*str) uart_putchar(*str++);
}

char uart_getchar(void) {
    volatile unsigned char *lsr = (volatile unsigned char *)(UART_BASE + 5);
    volatile unsigned char *data = (volatile unsigned char *)UART_BASE;
    unsigned char status = *lsr;
    if (status & 0x01) {
        return *data;
    }
    else {
    return '\0';
    }
}