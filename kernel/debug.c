#include "include/uart.h"
#include "include/kstring.h"
#include "include/panic.h"
#include "include/ansi.h"

#ifdef DEBUG

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winfinite-recursion"

static void __attribute__((noinline)) cause_stack_overflow(uint64_t depth) {
    volatile char dummy[128];
    uintptr_t sp = (uintptr_t)&dummy;

    extern char _stack_bottom[];
    uintptr_t stack_limit = (uintptr_t)_stack_bottom;

    if (sp < stack_limit + 256) {
        switch_to_emergency("Stack overflow detected!\n");
        return;
    }

    if (sp < stack_limit + 512) {
        print(ANSI_RED ANSI_BOLD "Stack exhausted at depth ");
        print_hex(depth);
        print("\r\n" ANSI_RESET);
    }

    dummy[0] = (char)depth;
    print_hex(sp);
    print("\r\n");
    cause_stack_overflow(depth + 1);
}

void debug_trigger_fault(int test_id) {
    switch (test_id) {
    case 1:
        print("[DEBUG] Illegal Instruction...\r\n");
        asm volatile("unimp");
        break;
    case 2: {
        print("[DEBUG] Load Access Fault...\r\n");
        volatile uint64_t *bad_ptr = (volatile uint64_t *)0x0;
        uint64_t dummy = *bad_ptr;
        (void)dummy;
        break;
    }
    case 3:
        print("[DEBUG] Stack Overflow (testing emergency stack)...\r\n");
        cause_stack_overflow(1);
        break;
    case 4:
        panic("User-triggered panic (DEBUG)");
        break;
    default:
        print("Usage: panic <1|2|3|4>\r\n");
        print("  1: Illegal Instruction\r\n");
        print("  2: Load Access Fault (0x0)\r\n");
        print("  3: Stack Overflow\r\n");
        print("  4: User-triggered panic\r\n");
        break;
    }
}

#pragma GCC diagnostic pop

#endif
