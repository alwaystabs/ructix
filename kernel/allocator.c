#include "include/memory.h"
#include <stdint.h>

extern char _heap_start[];
extern char _heap_end[];
static char *heap_ptr;
extern char buf[32];
void kmalloc_init(void) {
    heap_ptr = _heap_start;

}
void *kmalloc(size_t size) {
    size_t aligned_size = (size + 7) & ~7; // Formula allocates 8 bytes anyways (but 0 gives 0), which is required by RISC-V
    if (heap_ptr + aligned_size > _heap_end) { // Overflow
        panic("Out of Memory\n");
    }
    void *ptr = heap_ptr;
    heap_ptr += aligned_size;
    print("Requested ");
    itoa(size, buf);
    print(buf);
    print(" bytes, allocated ");
    itoa(aligned_size, buf);
    print(buf);
    print(" bytes");
    return ptr;
}
void kfree(void *ptr) {
    (void)ptr;
    return;
}