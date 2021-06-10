#pragma once

#include "util_defs.h"
#include <libdragon.h>

typedef struct {
	Position *positions;
	sprite_t *sprite;
	size_t qty;
	Size size;
	Position render_offset;
} SpriteBatch;

SpriteBatch *sprite_batch_init(sprite_t *sprite, size_t qty, Size size, Position render_offset);
void sprite_batch_draw(SpriteBatch *sprite_batch, int offset);
