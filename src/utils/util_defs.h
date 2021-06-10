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

inline Position new_position_zero() {
	Position pos;
	pos.x = 0;
	pos.y = 0;
	return pos;
}

inline Position new_position_same(float x_and_y) {
	Position pos;
	pos.x = x_and_y;
	pos.y = x_and_y;
	return pos;
}

inline Position new_position(float x, float y) {
	Position pos;
	pos.x = x;
	pos.y = y;
	return pos;
}

inline Size new_size_zero() {
	Size size;
	size.width = 0;
	size.height = 0;
	return size;
}

inline Size new_size_same(float width_and_height) {
	Size size;
	size.width = width_and_height;
	size.height = width_and_height;
	return size;
}

inline Size new_size(float width, float height) {
	Size size;
	size.width = width;
	size.height = height;
	return size;
}

inline Rect new_rect(Position pos, Size size) {
	Rect rect;
	rect.pos = pos;
	rect.size = size;
	return rect;
}
