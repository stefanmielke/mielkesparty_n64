#include "minigame_play_screen.h"

#include <stdio.h>

#include <libdragon.h>

#include "../definitions.h"
#include "screen_defs.h"
#include "../minigames.h"
#include "../../libs/libdragon-extensions/include/mem_pool.h"
#include "../minigames/minigame_flyingbats.h"

typedef struct {
	fnGameCreate create;
	fnGameTick tick;
	fnGameDisplay display;
	fnGameDestroy destroy;
	char *print;
} MiniGameScreen;

MiniGameScreen *play_screen_data;

void minigame_play_screen_create() {
	play_screen_data = mem_zone_alloc(&memory_pool, sizeof(MiniGameScreen));
	audio_load_sfx_set(SfxSet_Game);

	switch (selected_minigame) {
		case MINIGAME_FLYINGBATS:
			play_screen_data->create = &minigame_flyingbats_create;
			play_screen_data->tick = &minigame_flyingbats_tick;
			play_screen_data->display = &minigame_flyingbats_display;
			play_screen_data->destroy = &minigame_flyingbats_destroy;
			break;
		default:
			abort();
	}

	play_screen_data->create();
}

short minigame_play_screen_tick() {
	for (int i = 0; i < 4; ++i) {
		if (keys_released.c[i].start) {
			play_screen_data->destroy();
			return SCREEN_MINIGAME_DETAIL;
		}
	}

	if (!play_screen_data->tick()) {
		play_screen_data->destroy();
		return SCREEN_MINIGAME_DETAIL;
	}

	return SCREEN_MINIGAME_PLAY;
}

void minigame_play_screen_display(display_context_t disp) {
	play_screen_data->display(disp);

	/* Set the text output color */
	graphics_set_color(GRAY, TRANSP);
	graphics_draw_text(disp, SCREEN_BORDER, SCREEN_BOTTOM - 20, "START to exit");
}
