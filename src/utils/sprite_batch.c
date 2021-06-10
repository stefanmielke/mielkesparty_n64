#include "sprite_batch.h"

#include "../mielke_party.h"
#include <libdragon.h>

SpriteBatch *sprite_batch_init(sprite_t *sprite, size_t qty, Size size) {
	SpriteBatch *batch = mem_zone_alloc(&memory_pool, sizeof(SpriteBatch));
	batch->positions = mem_zone_alloc(&memory_pool, sizeof(Position) * qty);
	batch->sprite = sprite;
	batch->qty = qty;
	batch->size = size;

	return batch;
}

void sprite_batch_draw(SpriteBatch *sprite_batch, int offset) {
	rdp_sync(SYNC_PIPE);
	rdp_load_texture_stride(0, 0, MIRROR_DISABLED, sprite_batch->sprite, offset);

	Rect rect;
	rect.size = sprite_batch->size;
	for (size_t i = 0; i < sprite_batch->qty; ++i) {
		rect.pos = sprite_batch->positions[i];
		if (is_intersecting(rect, screen_rect)) {
			rdp_draw_sprite(0, rect.pos.x, rect.pos.y, MIRROR_DISABLED);
		}
	}
}
