#define MTIME    0x0200BFF8
#define MTIMECMP 0x02004000
#define TIMER_FREQ 10000000
#define TICKS_PER_SEC 60
#define TIMER_INTERVAL (TIMER_FREQ / TICKS_PER_SEC)
#include "include/uart.h"
#include "include/string.h"

volatile unsigned long long tick_count = 0;
extern void panic(const char *msg);

void kmain() {
    
    __asm__ volatile("csrsi mstatus, 0x8");
    __asm__ volatile("li t0, 0x80");
    __asm__ volatile("csrw mie, t0");

    unsigned long long now;
    __asm__ volatile("ld %0, 0(%1)" : "=r"(now) : "r"(MTIME));
    unsigned long long next = now + TIMER_INTERVAL;
    __asm__ volatile("sd %0, 0(%1)" : : "r"(next), "r"(MTIMECMP));

    uart_puts("Kernel started. Timer is ticking.\n");

    while (1) {
        __asm__ volatile("wfi");
        
        if (tick_count % 2 == 0) {
            uart_puts("Tick: ");
            char buf[32];
            itoa(tick_count, buf);
            uart_puts(buf);
            uart_putchar('\n');
        }
        if (tick_count >= 300) {
            panic("PANIC! Testing panic");
        }
    }
}