#define MTIME    0x0200BFF8
#define MTIMECMP 0x02004000
#define TIMER_FREQ 10000000
#define TICKS_PER_SEC 60
#define TIMER_INTERVAL (TIMER_FREQ / TICKS_PER_SEC)
#include "include/uart.h"
#include "include/string.h"
#include "include/memory.h"

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

    char buf[32];
    print("╔════════════════════════════════════╗\n");
    print("║          RUCTIX BOOT LOG           ║\n");
    print("╚════════════════════════════════════╝\n");
    print("Ructix is up. It's good.\n");
    print("---------------\n");
    print("Tick test.\n");
    for (int i = 0; i <= 20; i++) {
        print("Tick: ");
        itoa(i, buf);
        print(buf);
        uart_putchar('\n');
    }

    while (1) {
        __asm__ volatile("wfi");

        print("Tick test successful.\n");
        print("---------------\n");
        print("Memory allocation test.\n");

        kmalloc_init();
        print("Allocator init successful.\n");

        void *p1 = kmalloc(8);
        (void)p1; 
        print("Allocated 8 bytes by request of 8 on address ");
        itoa((unsigned long long)p1, buf);
        print(buf);
        print(" (in decimal view);\n");

        void *p2 = kmalloc(14);
        (void)p2;
        print("Allocated 16 bytes by request of 14 on address ");
        itoa((unsigned long long)p2, buf);
        print(buf);
        print(" (in decimal view);\n");
        

        void *p3 = kmalloc(27);
        (void)p3;
        print("Allocated 32 bytes by request of 27 on address ");
        itoa((unsigned long long)p3, buf);
        print(buf);
        print(" (in decimal view).\n");
        print("---------------\n");

        print("Panic memory allocation test.\n");
        void *p4 = kmalloc(8 * 1024 * 1024 + 1);
        (void)p4;
    }
}