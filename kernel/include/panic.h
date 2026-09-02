#ifndef PANIC_H
#define PANIC_H

void panic(const char *msg); // is in panic.S
void switch_to_emergency(const char *msg);

#endif