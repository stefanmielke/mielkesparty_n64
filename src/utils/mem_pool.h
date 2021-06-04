#pragma once

#include <malloc.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

// A contiguous zone where memory can be allocated.
struct mem_zone {
    uintptr_t pos;    // Pointer to current free space position.
    uintptr_t start;  // Pointer to start of zone.
    uintptr_t end;    // Pointer to end of zone.
};

// Allocate a memory zone with the given size.
void mem_zone_init(struct mem_zone *z, size_t size);

// Allocate memory from the zone.
void *mem_zone_alloc(struct mem_zone *z, size_t size);

// Free all objects in the zone.
void mem_zone_free_all(struct mem_zone *z);