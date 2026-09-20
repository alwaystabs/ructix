#include "include/tty.h"
#include "include/ansi.h"
#include "include/kstring.h"
#include "include/panic.h"
#include "include/uart.h"
#include "secret/rux.h"
#include "include/ructix_meta.h"
#include <stddef.h>
#include <stdint.h>

extern volatile unsigned long long tick_count; // look in timer.h
void debug_trigger_fault(int test_id); // look in debug.c
char cmd_buf[256]; // main buffer for commands i/o
int cmd_index = 0; // how many symbols are written now (by tty)

void cmd_help(const char *args);
void cmd_status(const char *args);
void cmd_panic(const char *args);
void cmd_67(const char *args);
void cmd_kalloc(const char *args);
void cmd_kfree(const char *args);

void print_prompt(void) {
  print("\033[K");
  print("ructix> ");
}

void tty_loop(void) {
  print_prompt();
  while (1) {
    __asm__ volatile("wfi");
    char c = uart_getchar();
    if (c != '\0') {
      if (c == '\r') { // '\r' = Enter key
        uart_putchar('\r');
        uart_putchar('\n');
        cmd_buf[cmd_index] = '\0';
        tty_execute(cmd_buf);
        cmd_index = 0;
        print_prompt();
      } else if (c == '\b' || c == 0x7f) { // '\b' and 0x7f = Backspace key
        if (cmd_index > 0) {
          cmd_index--;
          cmd_buf[cmd_index] = '\0'; // defining the end of the buffer (string). either we don't know where to write symbols
          uart_putchar('\b');
          uart_putchar(' ');
          uart_putchar('\b');
        }
      } else if (cmd_index < 255) {
        cmd_buf[cmd_index] = c;
        cmd_index++;
        uart_putchar(c);
      } else {
          // nothing - ignoring everything over 255 chars
      }
    }
  }
}

static const command_t commands[] = {
    {"help", cmd_help},
    {"status", cmd_status},
    {"panic", cmd_panic},
    {"67", cmd_67},
    {"kalloc", cmd_kalloc},
    {"kfree", cmd_kfree},
};

void tty_execute(volatile char *cmd) {
  const char *str_cmd = (const char *)cmd;

  if (*str_cmd == '\0') {
    if ((tick_count % 100) < 3) {
      rux_nothing_to_execute(); // in rux.c :)
    }
    return;
  }
  if (strnlen(cmd_buf, 255) >= 255) { // 256th symbol is '\0'
    print(ANSI_YELLOW ANSI_BOLD "[!] Command too long (max 255 chars); ignoring\n" ANSI_RESET);
    return;
  }

  const char *cmd_args = str_cmd;
  while (*cmd_args != ' ' && *cmd_args != '\0') {
      cmd_args++;
  }

  const char *args;
  if (*cmd_args == ' ') {
      *(char *)cmd_args = '\0';
      args = cmd_args + 1;
  } else {
      args = NULL;
  }

  *(char *)cmd_args = '\0';

  #ifdef DEBUG
  print(ANSI_DEBUG"\nFunc: ");
  print(str_cmd);
  print("; Args: ");
  if (args != NULL) {
      print(args);
  } else {
      print("(none)");
  }
  print(ANSI_RESET);
  print("\n");
  #endif

  for (int i = 0; i < (int)(sizeof(commands) / sizeof(commands[0])); i++) {
      if (strcmp(str_cmd, commands[i].name) == 0) {
          commands[i].func(args);
          return;
      }
  }

  print(ANSI_YELLOW ANSI_BOLD "[!] ructix: Unknown command ");
  print("'");
  print((const char *)(str_cmd));
  print("'\n" ANSI_RESET);
}

void cmd_status(const char *args) {
    (void)args;
    static char buf[32]; // for printing ticks

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

void cmd_help(const char *args) {
    (void)args;
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

void cmd_panic(const char *args) {
    (void)args;
#ifdef DEBUG
    if (args == NULL) {
        debug_trigger_fault(0);   // usage
        return;
    }
    if (strcmp(args, "1") == 0) { debug_trigger_fault(1); return; }
    if (strcmp(args, "2") == 0) { debug_trigger_fault(2); return; }
    if (strcmp(args, "3") == 0) { debug_trigger_fault(3); return; }
    if (strcmp(args, "4") == 0) { debug_trigger_fault(4); return; }
    debug_trigger_fault(0);   // if unknown - showing usage
#else
    char c;
    print(ANSI_YELLOW "Confirm: kernel will halt due to user-triggered panic (y/N) " ANSI_RESET);
    while ((c = uart_getchar()) == '\0') {}
    if (c == 'y') {
        panic("User-triggered panic");
    } else {
        print(ANSI_GRAY "Panic cancelled.\n" ANSI_RESET);
    }
#endif
}

void cmd_67(const char *args) {
  (void)args;
  print(ANSI_WARN ANSI_BOLD "How funny.\n" ANSI_RESET);
}
void cmd_kalloc(const char *args) {
    (void)args;
    // TODO: decimal to string
    print(ANSI_WARN"[!] In development \n" ANSI_RESET);
}
void cmd_kfree(const char *args) {
    (void)args;
    // TODO: implement kfree when allocator API is stable
    print(ANSI_WARN"[!] In development \n" ANSI_RESET);
}
