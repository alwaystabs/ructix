#include "include/timer.h"
#include "include/memory.h"
#include "include/tty.h"
#include "include/uart.h"
#include "include/ructix_meta.h"
#include "include/ansi.h"
#include "include/panic.h"
#include <stdint.h>

volatile unsigned long long tick_count = 0;
char kernel_buf[64]; // little buffer for kernel output
uintptr_t __stack_chk_guard = 0;

extern void trap_entry(void);
extern void test_allocator(void);

void __stack_chk_fail(void) { // -fstack-protector in compiler
    panic("Stack CORRUPTION detected!");
}

void init_check(void) {
    #ifdef DEBUG // logically. testing allocator should not take prod time
        test_allocator();
    #endif
    return;
}

void kmain(void) {
    unsigned long long now;
    __asm__ volatile("ld %0, 0(%1)" : "=r"(now) : "r"(MTIME));
    unsigned long long next = now + TIMER_INTERVAL;
    __asm__ volatile("sd %0, 0(%1)" : : "r"(next), "r"(MTIMECMP));

    uint64_t time;
    asm volatile("rdtime %0" : "=r"(time)); // predictable by rdtime, i know
    __stack_chk_guard = time ^ 0xDEADBEEF;

    print(ANSI_GRAY "╔════════════════════════════════════╗\n");
    print("║          RUCTIX BOOT LOG           ║\n");
    print("╚════════════════════════════════════╝\n" ANSI_RESET);
    print(ANSI_GRAY "built by alwaystabs in 2026.\n" ANSI_RESET);
    print(ANSI_GREEN "RUCTiX " RUCTIX_VERSION_STRING "! \n" ANSI_RESET);
    kmalloc_init();
    init_check();
    tty_loop();
}
