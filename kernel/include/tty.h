#ifndef TTY_H
#define TTY_H

typedef struct {
  const char *name;
  void (*func)(const char *args);
} command_t;

void tty_execute(volatile char *cmd);
void tty_loop(void);

#endif