#include "minigame_detail_screen.h"

#include <libdragon.h>
#include <stdio.h>

#include "../definitions.h"
#include "../gfx_h/gfx_interface.h"
#include "../gfx_h/gfx_minigame_detail.h"
#include "../minigames.h"
#include "../../libs/libdragon-extensions/include/mem_pool.h"
#include "screen_defs.h"

sprite_t *md_sprites;
sprite_t *md_thumb;
char *det_sc_time_string;
const char *get_minigame_name(int *offset_x);
const char *get_minigame_record();
const char *get_minigame_description();
void open_minigame_thumb();
void draw_minigame_controls(display_context_t disp, int start_x, int start_y);

void minigame_detail_screen_create() {
	det_sc_time_string = mem_zone_alloc(&memory_pool, sizeof(char) * 9);  // format Record: mm:ss\0
	for (size_t i = 0; i < 4; ++i) {
		players_ready[i] = false;
	}
	alloc_and_load_spritesheet_minigame_detail(md_sprites);
	open_minigame_thumb();

	audio_load_and_play_bgm(audio_player, BGM_INTRO, "/sfx/intro.raw");
	audio_load_sfx_set(SfxSet_Menu);
}

short minigame_detail_screen_tick() {
	for (int i = 0; i < 4; ++i) {
		if (players_ready[i]) {
			if (keys_released.c[i].start) {
				free(md_sprites);
				md_sprites = NULL;
				free(md_thumb);
				md_thumb = NULL;
				return SCREEN_MINIGAME_PLAY;
			}
			if (keys_released.c[i].B) {
				PLAY_AUDIO(SFX_UNCONFIRM);
				players_ready[i] = false;
			}
		} else {
			if (keys_released.c[i].A) {
				PLAY_AUDIO(SFX_CONFIRM);
				players_ready[i] = true;
			}
			if (keys_released.c[i].B) {
				free(md_thumb);
				md_thumb = NULL;
				free(md_sprites);
				md_sprites = NULL;
				PLAY_AUDIO(SFX_BACK);
				return SCREEN_INFINITE_MENU;
			}
		}
	}

	return SCREEN_MINIGAME_DETAIL;
}

void minigame_detail_screen_display(display_context_t disp) {
	{  // name
		int offset_x;
		graphics_set_color(BLUE, TRANSP);
		const char *name = get_minigame_name(&offset_x);
		graphics_draw_text(disp, SCREEN_LEFT + offset_x, SCREEN_TOP + 5, name);
	}
	{  // thumb
		graphics_draw_sprite(disp, 5, SCREEN_TOP + 15, md_thumb);
	}
	{  // record
		graphics_set_color(GREEN, TRANSP);
		graphics_draw_sprite_trans_stride(disp, RES_X / 2 - 14, SCREEN_TOP + 15, ui_sprites,
										  SPRITE_record);
		graphics_draw_text(disp, RES_X / 2 + 4, SCREEN_TOP + 20, get_minigame_record());
	}
	{  // description
		graphics_set_color(WHITE, TRANSP);
		graphics_draw_text(disp, SCREEN_LEFT, RES_Y / 2 + 60, get_minigame_description());
	}

	// controls
	draw_minigame_controls(disp, RES_X / 2 + 52, SCREEN_TOP + 30);

	{  // player state
		int start_x = RES_X / 2 + 67;
		int start_y = SCREEN_BOTTOM - 90;
		int offset = 38;
		int p1_sprite_offset = !connected_controllers[0] ? SPRITE_player_1_disconnected
							   : players_ready[0]		 ? SPRITE_player_1_ready
														 : SPRITE_player_1_not_ready;
		graphics_draw_sprite_trans_stride(disp, start_x, start_y, md_sprites, p1_sprite_offset);

		int p2_sprite_offset = !connected_controllers[1] ? SPRITE_player_2_disconnected
							   : players_ready[1]		 ? SPRITE_player_2_ready
														 : SPRITE_player_2_not_ready;
		graphics_draw_sprite_trans_stride(disp, start_x + offset, start_y, md_sprites,
										  p2_sprite_offset);

		int p3_sprite_offset = !connected_controllers[2] ? SPRITE_player_3_disconnected
							   : players_ready[2]		 ? SPRITE_player_3_ready
														 : SPRITE_player_3_not_ready;
		graphics_draw_sprite_trans_stride(disp, start_x, start_y + offset, md_sprites,
										  p3_sprite_offset);

		int p4_sprite_offset = !connected_controllers[3] ? SPRITE_player_4_disconnected
							   : players_ready[3]		 ? SPRITE_player_4_ready
														 : SPRITE_player_4_not_ready;
		graphics_draw_sprite_trans_stride(disp, start_x + offset, start_y + offset, md_sprites,
										  p4_sprite_offset);
	}

	{  // ready text
		bool any_ready = false;
		for (size_t i = 0; i < 4; ++i) {
			if (players_ready[i]) {
				any_ready = true;
				break;
			}
		}

		if (any_ready) {
			graphics_set_color(RED, TRANSP);
			graphics_draw_text(disp, RES_X / 2 + 60, SCREEN_BOTTOM - 10, "PRESS START");
		}
	}

	DRAW_BACK_BUTTON();
}

const char *get_minigame_name(int *offset_x) {
	*offset_x = 0;
	switch (selected_minigame) {
		case MINIGAME_FLYINGBATS:
			*offset_x = 50;
			return "Flying Bats";
		case MINIGAME_JUMPROPE:
			*offset_x = 50;
			return "Jump Rope";
		default:
			return "No MiniGame selected";
	}
}

const char *get_minigame_record() {
	switch (selected_minigame) {
		case MINIGAME_FLYINGBATS: {
			int time = game_save.body.times[MINIGAME_FLYINGBATS - 1];
			size_t minutes = time / 60;
			size_t seconds = time % 60;
			snprintf(det_sc_time_string, 8, "%02d:%02d", minutes, seconds);
			return det_sc_time_string;
		}
		case MINIGAME_JUMPROPE: {
			int time = game_save.body.times[MINIGAME_JUMPROPE - 1];
			size_t minutes = time / 60;
			size_t seconds = time % 60;
			snprintf(det_sc_time_string, 8, "%02d:%02d", minutes, seconds);
			return det_sc_time_string;
		}
		default:
			return "None";
	}
}

const char *get_minigame_description() {
	switch (selected_minigame) {
		case MINIGAME_FLYINGBATS:
			return " Fly across the screen\n\nwhile dodging the wisps!";
		case MINIGAME_JUMPROPE:
			return "Keep jumping the rope!";
		default:
			return "No description when\n\nthere's no minigame";
	}
}

void draw_minigame_controls(display_context_t disp, int start_x, int start_y) {
	graphics_set_color(WHITE, TRANSP);

	switch (selected_minigame) {
		case MINIGAME_FLYINGBATS: {
			start_x += 20;
			graphics_draw_sprite_trans_stride(disp, start_x, start_y, ui_sprites, SPRITE_joystick);
			graphics_draw_sprite_trans_stride(disp, start_x, start_y + 17, ui_sprites, SPRITE_dpad);
			graphics_draw_text(disp, start_x + 24, start_y + 14, "Move");

			graphics_draw_sprite_trans_stride(disp, start_x, start_y + 50, ui_sprites,
											  SPRITE_buttonA);
			graphics_draw_text(disp, start_x + 24, start_y + 54, "Fly");
		} break;
		case MINIGAME_JUMPROPE: {
			start_x += 20;
			graphics_draw_sprite_trans_stride(disp, start_x, start_y + 50, ui_sprites,
											  SPRITE_buttonA);
			graphics_draw_text(disp, start_x + 24, start_y + 54, "Jump");
		} break;
		default: {
			graphics_draw_text(disp, start_x, start_y, "No controls");
		} break;
	}
}

void open_minigame_thumb() {
	const char *sprite_name;
	switch (selected_minigame) {
		case MINIGAME_FLYINGBATS:
			sprite_name = "/flying_bats_large.sprite";
			break;
		case MINIGAME_JUMPROPE:
			sprite_name = "/jump_rope_large.sprite";
			break;
		default:
			return;
	}

	int fp = dfs_open(sprite_name);
	md_thumb = (sprite_t *)malloc(dfs_size(fp));
	dfs_read(md_thumb, 1, dfs_size(fp), fp);
	dfs_close(fp);
}
