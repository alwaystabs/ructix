#include "include/memory.h"
#include "include/panic.h"
#include <stdint.h>

extern char _heap_start[];
extern char _heap_end[];
static char *heap_ptr;
char alloc_buf[32];
void kmalloc_init(void) {
  heap_ptr = (char *)(((uintptr_t)_heap_start + 7) & ~7);
  ;
}
void *kmalloc(size_t size) {
  size_t aligned_size =
      (size + 7) & ~7; // Formula allocates 8 bytes anyways (but 0 gives 0),
                       // which is required by RISC-V
  if (heap_ptr + aligned_size > _heap_end) { // Overflow
    panic("Out of Memory\n");
  }
  void *ptr = heap_ptr;
  heap_ptr += aligned_size;
  #ifdef DEBUG
  print("Requested ");
  itoa(size, alloc_buf);
  print(alloc_buf);
  print(" bytes, allocated ");
  itoa(aligned_size, alloc_buf);
  print(alloc_buf);
  print(" bytes");
  #endif
  return ptr;
}
void kfree(void *ptr) {
  (void)ptr;
  return;
}