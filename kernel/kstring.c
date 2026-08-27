#include "include/kstring.h"

void reverse(
    char *buf) { // itoa produces reversed numbers, SO reverse() is needed
  int i = 0;
  int len = strlen(buf);
  int j = len - 1;
  char tmp;
  while (i < j) {
    tmp = buf[i];
    buf[i] = buf[j];
    buf[j] = tmp;
    i++;
    j--;
  }
}

void itoa(unsigned long long val, char *buf) {
  int i = 0;
  if (val == 0) { // Handle zero separately to avoid empty string
    buf[0] = '0';
    buf[1] = '\0';
    return;
  }
  while (val > 0) {
    int digit = val % 10; // Last
    buf[i] = '0' + digit;
    val = val / 10;
    i++;
  }
  buf[i] = '\0';
  reverse(buf);
}

size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len] != '\0') {
    len++;
  }
  return len;
}

size_t strnlen(const char *str, size_t max_len) {
  size_t len = 0;
  while (len < max_len && str[len] != '\0') {
    len++;
  }
  return len;
}

int strcmp(const char *str1, const char *str2) {
  while (*str1 && *str2 && *str1 == *str2) {
    str1++;
    str2++;
  }
  return *str1 - *str2;
}

void print_hex(uint64_t val) {
  char buf[19];
  buf[0] = '0';
  buf[1] = 'x';
  int idx = 2;
  for (int i = 15; i >= 0; i--) {
    uint8_t nibble = (val >> (i * 4)) & 0xF;
    if (nibble < 10) {
      buf[idx] = '0' + nibble;
    } else {
      buf[idx] = 'a' + (nibble - 10);
    }
    idx++;
  }
  buf[18] = '\0';
  print(buf);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  while (n > 0 && *s1 && (*s1 == *s2)) {
    s1++;
    s2++;
    n--;
  }
  if (n == 0) {
    return 0;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}