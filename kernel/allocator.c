#include "include/memory.h"

extern char _heap_start[];
extern char _heap_end[];
static char *heap_ptr;
void kmalloc_init(void) {
    heap_ptr = _heap_start;

}
void *kmalloc(size_t size) {
    size_t aligned_size = (size + 7) & ~7; // 0 requested bytes -> 0 allocated bytes. Simple and good!
    char buf[32];
    void *ptr = heap_ptr;
    if (heap_ptr + aligned_size > _heap_end) {
        panic("[    PANIC!!!    ]      Out of Memory\n");
    }
    heap_ptr += aligned_size;
    return ptr;
}
void free(void *ptr) {
    return;
    // Does nothing. Bump allocator is limited to allocating only.
    // But I'll improve it so it can free allocated memory! :D
}