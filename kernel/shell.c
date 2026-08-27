#include "include/shell.h"

#include "include/kstring.h"
#include "include/panic.h"
#include "include/uart.h"
#include "secret/rux.h"

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
  print("Ructix shell is UP. \n");
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
  } else if (strncmp(str_cmd, "panic 2", 7) == 0) {
    debug_trigger_fault(2);
  } else if (strncmp(str_cmd, "panic 3", 7) == 0) {
    debug_trigger_fault(3);
  } else if (strncmp(str_cmd, "panic", 5) == 0) {
    debug_trigger_fault(0);
  }
#endif
  if (*str_cmd == '\0') {
    if ((tick_count % 100) < 3) {
      rux_nothing_to_execute();
    }
    return;
  }
  if (strnlen(str_cmd, 255) >= 255) {
    print("Command too long (max 255 chars); ignoring\n");
    return;
  }

  for (int i = 0; i < (int)(sizeof(commands) / sizeof(commands[0])); i++) {
    if (strcmp(str_cmd, commands[i].name) == 0) {
      commands[i].func();
      return;
    }
  }
  print("ructix: Unknown command ");
  print("'");
  print((const char *)(str_cmd));
  print("'\n");
}

void cmd_help(void) {
  print("help - show this message\r\n");
  print("status - show system status (ticks, memory)\r\n");
  print("panic - trigger kernel panic\n");
#ifdef DEBUG
  print("panic 1 - unimp (RISC-V reserved illegal instruction)\r\n");
  print("panic 2 - Load Access Fault\r\n");
  print("panic 3 - Stack Overflow\r\n");
#endif
}

void cmd_status(void) {
  static char buf[32];
  print("Ticks: ");
  itoa(tick_count, buf);
  print(buf);
  print("\n");
}

void cmd_panic(void) {
#ifndef DEBUG
char c;
print("Confirm: kernel will halt due to user-triggered panic (y/N)\n");
while ((c = uart_getchar()) == '\0') {

}
if (c == 'y') {
        panic("User-triggered panic");
    }
#endif
}
