// kernel/init.c
#include "include/timer.h"
#include "include/kstring.h"
#include "include/memory.h"
#include "include/shell.h"
#include "include/uart.h"
#include "include/ructix_meta.h"
#include "include/ansi.h"
#include "include/panic.h"
#include <stdint.h>

volatile unsigned long long tick_count = 0;
char kernel_buf[64];
uintptr_t __stack_chk_guard = 0;

extern void trap_entry(void);
extern void test_allocator(void);

void __stack_chk_fail(void) {
    panic("Stack CORRUPTION detected!");
}

void init_check(void) {
    #ifdef DEBUG
        test_allocator();
    #endif
    return;
}

void kmain(void) {
    __asm__ volatile("csrw mtvec, %0" : : "r"(trap_entry));
    __asm__ volatile("csrsi mstatus, 0x8");
    __asm__ volatile("li t0, 0x80");
    __asm__ volatile("csrw mie, t0");

    unsigned long long now;
    __asm__ volatile("ld %0, 0(%1)" : "=r"(now) : "r"(MTIME));
    unsigned long long next = now + TIMER_INTERVAL;
    __asm__ volatile("sd %0, 0(%1)" : : "r"(next), "r"(MTIMECMP));

    uint64_t time;
    asm volatile("rdtime %0" : "=r"(time));
    __stack_chk_guard = time ^ 0xDEADBEEF;

    print(ANSI_GRAY "╔════════════════════════════════════╗\n");
    print("║          RUCTIX BOOT LOG           ║\n");
    print("╚════════════════════════════════════╝\n" ANSI_RESET);
    print(ANSI_GRAY "built by alwaystabs in 2026.\n" ANSI_RESET);
    print(ANSI_GREEN "Ructix v0.2.0!\n" ANSI_RESET);

    init_check();

    shell_loop();
}