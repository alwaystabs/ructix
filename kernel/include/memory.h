#ifndef MEMORY_H
#define MEMORY_H

#include "kstring.h"
#include "panic.h"
#include "uart.h"
#include <stddef.h>

typedef struct block_header {
    size_t size;
    struct block_header *next;
    int used;
} block_header_t;

void kmalloc_init(void);
void *kmalloc(size_t size);
void kfree(void *ptr);
void test_allocator(void);

#endif