#include "include/shell.h"
#include "include/ansi.h"
#include "include/kstring.h"
#include "include/panic.h"
#include "include/uart.h"
#include "secret/rux.h"
#include "include/ructix_meta.h"

extern volatile unsigned long long tick_count;
int strcmp(const char *str1, const char *str2);
void debug_trigger_fault(int test_id);
volatile char cmd_buf[256];
volatile int cmd_index = 0;
static int prompt_len = 0;

void cmd_help(void);
void cmd_status(void);
void cmd_panic(void);

char print_prompt(void) {
  const char *prompt = "ructix> ";
  int len = strlen(prompt);
  print("\033[K");
  print(prompt);
  return len;
}

void shell_loop(void) {
  prompt_len = print_prompt();
  while (1) {
    __asm__ volatile("wfi");
    char c = uart_getchar();
    if (c != '\0') {
      if (c == '\r') { // '\r' = Enter key
        uart_putchar('\r');
        uart_putchar('\n');
        cmd_buf[cmd_index] = '\0';
        shell_execute(cmd_buf);
        cmd_index = 0;
        print_prompt();
      } else if (c == '\b' || c == 0x7f) { // '\b' and 0x7f = Backspace key
        if (cmd_index > 0) {
          cmd_index--;
          uart_putchar('\b');
          uart_putchar(' ');
          uart_putchar('\b');
        }
      } else if (cmd_index < 255) {
        cmd_buf[cmd_index] = c;
        cmd_index++;
        uart_putchar(c);
      } else {
        uart_putchar(c);
      }
    }
  }
}

static const command_t commands[] = {
    {"help", cmd_help},
    {"status", cmd_status},
    {"panic", cmd_panic},
};

void shell_execute(volatile char *cmd) {
  const char *str_cmd = (const char *)cmd;
#ifdef DEBUG
  if (strncmp(str_cmd, "panic 1", 7) == 0) {
    debug_trigger_fault(1);
    return;
  } else if (strncmp(str_cmd, "panic 2", 7) == 0) {
    debug_trigger_fault(2);
    return;
  } else if (strncmp(str_cmd, "panic 3", 7) == 0) {
    debug_trigger_fault(3);
    return;
  } else if (strncmp(str_cmd, "panic", 5) == 0) {
    debug_trigger_fault(0);
    return;
  }
#endif
  if (*str_cmd == '\0') {
    if ((tick_count % 100) < 3) {
      rux_nothing_to_execute();
    }
    return;
  }
  if (strnlen(str_cmd, 255) >= 255) {
    print(ANSI_YELLOW ANSI_BOLD "[!] Command too long (max 255 chars); ignoring\n" ANSI_RESET);
    return;
  }

  for (int i = 0; i < (int)(sizeof(commands) / sizeof(commands[0])); i++) {
    if (strcmp(str_cmd, commands[i].name) == 0) {
      commands[i].func();
      return;
    }
  }
  
  print(ANSI_YELLOW ANSI_BOLD "[!] ructix: Unknown command ");
  print("'");
  print((const char *)(str_cmd));
  print("'\n" ANSI_RESET);
}

void cmd_status(void) {
    static char buf[32];

    print(ANSI_CYAN "═══════════════════════════════════════════════\n" ANSI_RESET);
    print(ANSI_BOLD "  System Status\n" ANSI_RESET);
    print(ANSI_CYAN "═══════════════════════════════════════════════\n" ANSI_RESET);

    print("  " ANSI_GRAY "Version:" ANSI_RESET "  ");
    print(RUCTIX_VERSION_STRING);
    print("\n");

    print("  " ANSI_GRAY "Build:" ANSI_RESET "   ");
    print(RUCTIX_BUILD_TYPE);
    print("\n");

    print("  " ANSI_GRAY "Ticks:" ANSI_RESET "   ");
    itoa(tick_count, buf);
    print(buf);
    print("\n");

    print(ANSI_CYAN "═══════════════════════════════════════════════\n" ANSI_RESET);
}

void cmd_help(void) {
    print(ANSI_CYAN "═══════════════════════════════════════════════════════════\n" ANSI_RESET);
    print(ANSI_BOLD "  RUCTiX " ANSI_RESET);
    print(RUCTIX_VERSION_STRING);
    print(ANSI_BOLD " — Available Commands\n" ANSI_RESET);
    print(ANSI_CYAN "═══════════════════════════════════════════════════════════\n" ANSI_RESET);

    print(ANSI_GREEN "  help" ANSI_RESET);
    print("    — Show this help message\n");

    print(ANSI_GREEN "  status" ANSI_RESET);
    print("  — Show system status (ticks, memory)\n");

    print(ANSI_GREEN "  panic" ANSI_RESET);
    print("   — Trigger a kernel panic\n");

#ifdef DEBUG
    print(ANSI_YELLOW "\n  Debug commands:\n" ANSI_RESET);
    print(ANSI_GREEN "  panic 1" ANSI_RESET);
    print("  — Illegal instruction\n");
    print(ANSI_GREEN "  panic 2" ANSI_RESET);
    print("  — Load access fault\n");
    print(ANSI_GREEN "  panic 3" ANSI_RESET);
    print("  — Stack overflow\n");
#endif

    print(ANSI_CYAN "═══════════════════════════════════════════════════════════\n" ANSI_RESET);
    print(ANSI_GRAY "  Type a command and press Enter.\n" ANSI_RESET);
}

void cmd_panic(void) {
#ifndef DEBUG
    char c;
    print(ANSI_YELLOW "Confirm: kernel will halt due to user-triggered panic (y/N)\n" ANSI_RESET);
    while ((c = uart_getchar()) == '\0') {}
    if (c == 'y') {
        panic("User-triggered panic");
    } else {
        print(ANSI_GRAY "Panic cancelled.\n" ANSI_RESET);
    }
#endif
}
