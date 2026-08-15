#include "include/shell.h"

#include "include/uart.h"
#include "include/panic.h"
#include "include/kstring.h"
#include "secret/rux.h"

extern volatile unsigned long long tick_count;
int strcmp(const char *str1, const char *str2);
char cmd_buf[256];
int cmd_index = 0;
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
            }
            else if (c == '\b' || c == 0x7f) { // '\b' and 0x7f = Backspace key
                if (cmd_index > 0) {
                    cmd_index--;
                    uart_putchar('\b');
                    uart_putchar(' ');
                    uart_putchar('\b');
                }
            }
        else if (cmd_index <= 255) {
            cmd_buf[cmd_index] = c;
            cmd_index++;
            uart_putchar(c);
        }
        else {
            uart_putchar(c);
        }
        }
    }
}

static const command_t commands[] = {
    {"help",   cmd_help},
    {"status", cmd_status},
    {"panic",  cmd_panic},
};

void shell_execute(char *cmd) {
    if (*cmd == '\0') {
        if ((tick_count % 100) < 3) {
            rux_nothing_to_execute();
        }
        return;
    }
    if (strlen(cmd) >= 255) {
        print("Command too long (max 255 chars)\n");
        return;
    }

    for (int i = 0; i < (int)(sizeof(commands) / sizeof(commands[0])); i++) {
        if (strcmp(cmd, commands[i].name) == 0) {
            commands[i].func();
            return;
        }
    }
    print("ructix: Unknown command ");
    print("'");
    print((const char *)(cmd));
    print("'\n");
}

void cmd_help(void) {
    print("help - show this message\n");
    print("status - show system status (ticks, memory)\n");
    print("panic - trigger kernel panic\n");
}

void cmd_status(void) {
    static char buf[32];
    print("Ticks: ");
    itoa(tick_count, buf);
    print(buf);
    print("\n");
}

void cmd_panic(void) {
    panic("User-triggered panic");
}