#pragma once

#include <stdbool.h>

typedef struct position {
    float x;
    float y;
} Position;

typedef struct size {
    float width;
    float height;
} Size;

typedef struct rect {
    Position pos;
    Size size;
} Rect;

bool is_intersecting(Rect a, Rect b);
bool contains(Rect inner, Rect outer);
