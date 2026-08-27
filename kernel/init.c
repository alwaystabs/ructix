#include "include/timer.h"
#include "include/kstring.h"
#include "include/memory.h"
#include "include/shell.h"
#include "include/uart.h"
#include <stdint.h>

volatile unsigned long long tick_count = 0;
extern void panic(const char *msg); // in boot.S
char kernel_buf[64];
extern void trap_entry(void);

void init_check(void) {
  print("---------------\n");
  print("Tick test.\n");
  for (int i = 0; i <= 20; i++) { // 20 ticks
    print("Tick: ");
    itoa(i, kernel_buf);
    print(kernel_buf);
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
  print_hex((unsigned long long)p1); // Hex now!
  uart_putchar('\n');

  void *p2 = kmalloc(14);
  (void)p2;
  print(" on address ");
  print_hex((unsigned long long)p2);
  uart_putchar('\n');

  void *p3 = kmalloc(27);
  (void)p3;
  print(" on address ");
  print_hex((unsigned long long)p3);
  uart_putchar('\n');
  print("---------------\n");
}

#ifdef DEBUG

extern char _stack_bottom[];
#define STACK_LIMIT_ADDR ((uintptr_t)_stack_bottom)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winfinite-recursion"

static void __attribute__((noinline)) cause_stack_overflow(uint64_t depth) {
  volatile char dummy[128];
  if ((uintptr_t)&dummy[0] < STACK_LIMIT_ADDR) {
    print("[PANIC] Stack exhausted at depth ");
    print_hex(depth);
    print("\r\n");
  }
  dummy[0] = (char)depth;
  print_hex((uintptr_t)&dummy[0]);
  print("\r\n");
  cause_stack_overflow(depth + 1);
}

void debug_trigger_fault(int test_id) {
  if (test_id >= 5) {
    print("Unknown DEBUG panic option; ignoring\r\n");
    return;
  }
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
    print("[DEBUG] User-triggered panic");
    panic("");

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
void kmain() {
  __asm__ volatile("csrw mtvec, %0" : : "r"(trap_entry));
  __asm__ volatile("csrsi mstatus, 0x8"); // Enabling interrputs by MIE bit.
                                          // (RISC-V Privileged, 3.1.9)
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
  print("\033[90mbuilt by alwaystabs in 2026.\033[0m\n");
  print("\033[32mRuctix v0.2.0!\033[0m\n");
  init_check();
  shell_loop();
}