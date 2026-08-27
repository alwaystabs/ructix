#ifndef KSTRING_H
#define KSTRING_H
#include "uart.h"
#include <stddef.h>
#include <stdint.h>

void reverse(char *buf);
void itoa(unsigned long long val, char *buf);
size_t strlen(const char *str);
size_t strnlen(const char *str, size_t max_len);
int strcmp(const char *str1, const char *str2);
void print_hex(uint64_t val);
int strncmp(const char *s1, const char *s2, size_t n);

#endif