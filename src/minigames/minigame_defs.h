#pragma once

typedef struct position {
    float x;
    float y;
} Position;

typedef struct size {
    float width;
    float height;
} Size;

extern MemZone memory_pool;

extern struct controller_data keys_held;
extern struct controller_data keys_released;
extern int connected_controllers;
