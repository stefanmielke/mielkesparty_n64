#include "main_screen.h"

#include <libdragon.h>

#include "screen_defs.h"

void main_screen_create() {
	audio_load_and_play_bgm(audio_player, BGM_INTRO, "/sfx/intro.raw");
	audio_load_sfx_set(SfxSet_Menu);
}

short main_screen_tick() {
	for (int i = 0; i < 4; ++i) {
		if (keys_released.c[i].start) {
			PLAY_AUDIO(SFX_CONFIRM);
			return SCREEN_MAIN_MENU;
		}
	}

	return SCREEN_MAIN;
}

void main_screen_display(display_context_t disp) {
	graphics_set_color(BLUE, BLACK);
	graphics_draw_text(disp, (RES_X / 2) - 55, (RES_Y / 2) - 20, "Mielke's Party");

	graphics_set_color(RED, BLACK);
	graphics_draw_text(disp, (RES_X / 2) - 45, (RES_Y / 2) + 20, "Press START");
}
