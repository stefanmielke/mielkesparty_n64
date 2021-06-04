#include "mem_pool.h"

void mem_zone_init(struct mem_zone *z, size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        abort(); // Put your error handling here.
    }
    z->pos = (uintptr_t)ptr;
    z->start = (uintptr_t)ptr;
    z->end = (uintptr_t)ptr + size;
}

void *mem_zone_alloc(struct mem_zone *z, size_t size) {
    if (size == 0) {
        return NULL;
    }
    // Round up to multiple of 16 bytes.
    size = (size + 15) & ~(size_t)15;
    // How much free space remaining in zone?
    size_t rem = z->end - z->pos;
    if (rem < size) {
        abort(); // Out of memory. Put your error handling here.
    }
    uintptr_t ptr = z->pos;
    z->pos = ptr + size;
    return (void *)ptr;
}

void mem_zone_free_all(struct mem_zone *z) {
    z->pos = z->start;
}