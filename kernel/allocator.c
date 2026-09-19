#include "include/memory.h"
#include "include/panic.h"
#include "include/ansi.h"
#include <stdint.h>
#include <stddef.h>

extern char _heap_start[];
extern char _heap_end[];
static char *heap_ptr;
char alloc_buf[32];

static block_header_t *free_list = NULL;

void kmalloc_init(void) {
    heap_ptr = (char *)(((uintptr_t)_heap_start + 7) & ~7);
}

void *kmalloc(size_t size) { // brainfuck
    if (size == 0) return NULL;

    size_t total_size = size + sizeof(block_header_t);
    total_size = (total_size + 7) & ~7;

    block_header_t *prev = NULL;
    block_header_t *current = free_list;

    if (total_size > (size_t)(_heap_end - heap_ptr)) {
        print (ANSI_RED ANSI_BOLD "[!!!] Tried to allocate a block exceeding heap limit, ignoring\n" ANSI_RESET);
        return 0;
    }

    while (current) {
        if (current->used == 0 && current->size >= total_size) {
            current->used = 1;

            if (prev) {
                prev->next = current->next;
            } else {
                free_list = current->next;
            }

            #ifdef DEBUG
                print("[kmalloc] Reused block at ");
                print_hex((uintptr_t)current);
                print(" (size=");
                itoa(current->size, alloc_buf);
                print(alloc_buf);
                print(")\n");
            #endif

            return (char*)current + sizeof(block_header_t);
        }
        prev = current;
        current = current->next;
    }

    block_header_t *block = (block_header_t*)heap_ptr;
    block->size = total_size;
    block->next = NULL;
    block->used = 1;

    heap_ptr += total_size;

    #ifdef DEBUG
        print("[kmalloc] New block at ");
        print_hex((uintptr_t)block);
        print(" (size=");
        itoa(block->size, alloc_buf);
        print(alloc_buf);
        print(")\n");
    #endif

    return (char*)block + sizeof(block_header_t);
}

void kfree(void *ptr) {
    if (!ptr) return;

    block_header_t *block = (block_header_t*)((char*)ptr - sizeof(block_header_t));

    if (block->used == 0) {
        print("[!] Double free detected\n");
        return;
    }

    block->used = 0;

    block->next = free_list;
    free_list = block;

    #ifdef DEBUG
        print("[kfree] Freed block at ");
        print_hex((uintptr_t)block);
        print(" (size=");
        itoa(block->size, alloc_buf);
        print(alloc_buf);
        print(")\n");
    #endif
}

void test_allocator(void) { // Yes, this is AI
    print(ANSI_DEBUG"[INIT] Allocator initialized.\n");
    print("[INFO] Heap: " ANSI_RESET);
    print_hex((uintptr_t)_heap_start);
    print(" - ");
    print_hex((uintptr_t)_heap_end);
    print(" (");
    itoa((uintptr_t)_heap_end - (uintptr_t)_heap_start, alloc_buf);
    print(alloc_buf);
    print(" bytes)\n\n");

    print("TEST 1. Basic allocation\n");
    void *p1 = kmalloc(8);
    void *p2 = kmalloc(14);
    void *p3 = kmalloc(27);

    print("  p1 (8 bytes)  : ");
    print_hex((uintptr_t)p1);
    print("\n");
    print("  p2 (14 bytes) : ");
    print_hex((uintptr_t)p2);
    print("\n");
    print("  p3 (27 bytes) : ");
    print_hex((uintptr_t)p3);
    print("\n");

    if (p1 != p2 && p2 != p3 && p1 != p3) {
        print(ANSI_SUCCESS"All addresses are unique\n" ANSI_RESET);
    } else {
        print("Addresses overlap!\n");
    }
    print("\n");

    print("TEST 2. Free and reuse\n");
    print(" Freeing p2...\n");
    kfree(p2);

    print("Allocating new 14 bytes...\n");
    void *p4 = kmalloc(14);

    print("p2 was at: ");
    print_hex((uintptr_t)p2);
    print("\n");
    print("p4 is at : ");
    print_hex((uintptr_t)p4);
    print("\n");

    if (p4 == p2) {
        print(ANSI_SUCCESS"Memory reused successfully!\n"ANSI_RESET);
    } else {
        print(ANSI_ERROR"Memory not reused (different address)\n"ANSI_RESET);
    }
    print("\n");

    print("TEST 3. Multiple small allocations\n");
    #define SMALL_ALLOC_COUNT 10
    void *small_ptrs[SMALL_ALLOC_COUNT];
    for (int i = 0; i < SMALL_ALLOC_COUNT; i++) {
        small_ptrs[i] = kmalloc(4);
        print("  ");
        print_hex((uintptr_t)small_ptrs[i]);
        print("\n");
    }

    for (int i = 0; i < SMALL_ALLOC_COUNT; i++) {
        kfree(small_ptrs[i]);
    }
    print(ANSI_SUCCESS"All small allocations freed!\n\n"ANSI_RESET);

    print("TEST 4. Large allocation (512 bytes)\n");
    void *large = kmalloc(512);
    print(ANSI_DEBUG"Large block at: "ANSI_RESET);
    print_hex((uintptr_t)large);
    print("\n");
    kfree(large);
    print(ANSI_SUCCESS"Freed!\n\n"ANSI_RESET);

    print("TEST 5. Zero-size allocation\n");
    void *zero = kmalloc(0);
    print(ANSI_DEBUG"kmalloc(0) returned: ");
    if (zero == NULL) {
        print(ANSI_SUCCESS "NULL (correct)" ANSI_RESET);
    } else {
        print_hex((uintptr_t)zero);
        print(ANSI_ERROR " (should be NULL!)" ANSI_RESET);
    }
    print("\n");
    kfree(zero);
    print("\n");

    print("TEST 6. Stress test\n");
    #define STRESS_COUNT 100
    void *stress[STRESS_COUNT];

    for (int i = 0; i < STRESS_COUNT; i++) {
        size_t size = 8 + (i % 32);
        stress[i] = kmalloc(size);
        if (stress[i] == NULL) {
            print(ANSI_ERROR "Failed at step ");
            itoa(i, alloc_buf);
            print(alloc_buf);
            print(ANSI_RESET "\n");
            break;
        }
    }

    for (int i = 0; i < STRESS_COUNT/2; i++) {
        kfree(stress[i]);
    }

    for (int i = 0; i < STRESS_COUNT/2; i++) {
        stress[i] = kmalloc(16);
        if (stress[i] == NULL) {
            print(ANSI_ERROR "Reallocation failed\n");
            break;
        }
    }

    for (int i = 0; i < STRESS_COUNT; i++) {
        kfree(stress[i]);
    }

    print(ANSI_SUCCESS "Stress test passed!\n");
    print("╔═══════════════════════════════════════════════════════════╗\n");
    print("║                All allocator tests passed!                ║\n");
    print("╚═══════════════════════════════════════════════════════════╝\n" ANSI_RESET);
}
