#ifndef SHELL_H
#define SHELL_H

typedef struct {
  const char *name;
  void (*func)(void);
} command_t;

void shell_execute(volatile char *cmd);
void shell_loop(void);

#endif