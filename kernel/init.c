#define MTIME    0x0200BFF8
#define MTIMECMP 0x02004000
#define TIMER_FREQ 10000000
#define TICKS_PER_SEC 60
#define TIMER_INTERVAL (TIMER_FREQ / TICKS_PER_SEC)
#include "include/uart.h"
#include "include/string.h"
#include "include/memory.h"

volatile unsigned long long tick_count = 0;
extern void panic(const char *msg); // in boot.S
char buf[32]; // Global buffer now

void init_check(void) {
        print("---------------\n");
        print("Tick test.\n");
        for (int i = 0; i <= 20; i++) { // 20 ticks for test
            print("Tick: ");
            itoa(i, buf);
            print(buf);
            uart_putchar('\n');
        }
        print("Tick test successful.\n");
        print("---------------\n");
        print("Memory allocation test.\n");

        kmalloc_init();
        print("Allocator init successful.\n");
        print("Allocating memory test.\n");
        void *p1 = kmalloc(8);
        (void)p1; // Or else GCC gives warning, the same to p2 and p3
        print(" on address ");
        itoa((unsigned long long)p1, buf); // Detecting allocated memory address, only decimal addresses for now. The same for others
        print(buf);
        print(" (in decimal view);\n");

        void *p2 = kmalloc(14);
        (void)p2;
        print(" on address ");
        itoa((unsigned long long)p2, buf);
        print(buf);
        print(" (in decimal view);\n");
        

        void *p3 = kmalloc(27);
        (void)p3;
        print(" on address ");
        itoa((unsigned long long)p3, buf);
        print(buf);
        print(" (in decimal view).\n");
        print("---------------\n");
    }

void kmain() {
    
    __asm__ volatile("csrsi mstatus, 0x8"); // Enabling interrputs by MIE bit. (RISC-V Privileged, 3.1.9)
    __asm__ volatile("li t0, 0x80");
    __asm__ volatile("csrw mie, t0"); 
    // ^^^^^^ Timer enabling by MTIE bit. (RISC-V Privileged, 3.2.1)
    unsigned long long now; // = current tick 
    __asm__ volatile("ld %0, 0(%1)" : "=r"(now) : "r"(MTIME));
    unsigned long long next = now + TIMER_INTERVAL; // next tick (now + interval)
    __asm__ volatile("sd %0, 0(%1)" : : "r"(next), "r"(MTIMECMP));

    print("╔════════════════════════════════════╗\n");
    print("║          RUCTIX BOOT LOG           ║\n");
    print("╚════════════════════════════════════╝\n");
    print("Ructix v0.0.7 is up. It's good.\n");
    init_check();
    print("Echo loop started, type something and you'll get echo of it\n");
    while (1) {
        __asm__ volatile("wfi");
        char c = uart_getchar();
        if (c != '\0') {
            if (c == '\r') { // '\r' = Enter key
                uart_putchar('\r');
                uart_putchar('\n');
            }
            else if (c == '\b' || c == 0x7f) { // '\b' and 0x7f = Backspace key
                uart_putchar('\b');
                uart_putchar(' ');
                uart_putchar('\b');
            }
            else {
                uart_putchar(c);
            }
        }
    }
}