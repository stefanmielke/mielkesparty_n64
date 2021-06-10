#pragma once

#include <stdbool.h>

typedef struct {
    float x;
    float y;
} Position;

typedef struct {
    float width;
    float height;
} Size;

typedef struct {
    Position pos;
    Size size;
} Rect;

bool is_intersecting(Rect a, Rect b);
bool contains(Rect inner, Rect outer);
