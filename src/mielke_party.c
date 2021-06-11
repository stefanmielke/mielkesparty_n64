#include "mielke_party.h"

#include <libdragon.h>

#include <string.h>

#include "definitions.h"
#include "screens/no_save_screen.h"
#include "screens/main_screen.h"
#include "screens/main_menu_screen.h"
#include "screens/infinite_menu_screen.h"
#include "screens/minigame_detail_screen.h"
#include "screens/minigame_play_screen.h"
#include "gfx_h/gfx_interface.h"

void change_screen(ScreenType next_screen);
void setup();

int main() {
	setup();

	while (1) {
		if (next_screen != SCREEN_NONE && screen_current != next_screen)
			change_screen(next_screen);

		// tick controller
		controller_scan();
		keys_held = get_keys_held();
		keys_released = get_keys_up();
		int controllers_present = get_controllers_present();
		connected_controllers[0] = controllers_present & CONTROLLER_1_INSERTED;
		connected_controllers[1] = controllers_present & CONTROLLER_2_INSERTED;
		connected_controllers[2] = controllers_present & CONTROLLER_3_INSERTED;
		connected_controllers[3] = controllers_present & CONTROLLER_4_INSERTED;

		next_screen = screen_tick();

		audio_tick(audio_player);

		// tick render
		static display_context_t disp = 0;
		while (!(disp = display_lock()))
			;

		if (next_screen != SCREEN_NONE && screen_current != next_screen) {
			graphics_fill_screen(disp, BLACK);
			graphics_set_color(WHITE, TRANSP);
			graphics_draw_text(disp, SCREEN_BORDER, RES_Y - SCREEN_BORDER - 20, "Loading...");
		} else {
			graphics_fill_screen(disp, BLACK);
			screen_display(disp);
		}

		// debug info
		if (keys_held.c[0].C_right) {
			struct mallinfo mem_info = mallinfo();
			char memory_used_text[21];
			snprintf(memory_used_text, 21, "%dKB/%dKB", mem_info.uordblks / 1024,
					 get_memory_size() / 1024);

			graphics_set_color(mem_info.uordblks > 4 * 1024 * 1024				 ? RED
							   : mem_info.uordblks > (get_memory_size() / 4) * 3 ? RED
																				 : GREEN,
							   TRANSP);
			graphics_draw_text(disp, SCREEN_BORDER + 20, SCREEN_BOTTOM - 12, memory_used_text);
		}

		display_show(disp);
	}
}

void setup() {
	screen_current = SCREEN_NONE;
	next_screen = SCREEN_MAIN;
	selected_minigame = MINIGAME_NONE;

	/* enable interrupts (on the CPU) */
	init_interrupts();

	/* Initialize peripherals */
	display_init(SCREEN_RESOLUTION, DEPTH_16_BPP, NUM_BUFFERS, GAMMA_NONE, ANTIALIAS_RESAMPLE);
	dfs_init(DFS_DEFAULT_LOCATION);
	rdp_init();
	controller_init();
	timer_init();
	audio_init(AUDIO_FREQUENCY, AUDIO_BUFFERS);

	mem_zone_init(&memory_pool, 0.6 * 1024);

	alloc_and_load_spritesheet_interface(ui_sprites);
	audio_player = audio_setup(AUDIO_FREQUENCY);

	TRANSP = graphics_make_color(0, 0, 0, 0);
	WHITE = graphics_make_color(255, 255, 255, 255);
	BLACK = graphics_make_color(0, 0, 0, 255);
	RED = graphics_make_color(255, 100, 100, 255);
	GREEN = graphics_make_color(100, 255, 100, 255);
	BLUE = graphics_make_color(100, 100, 255, 255);
	GRAY = graphics_make_color(100, 100, 100, 255);
	DARK_GRAY = graphics_make_color(40, 40, 40, 255);

	screen_rect.pos.x = SCREEN_LEFT;
	screen_rect.pos.y = SCREEN_TOP;
	screen_rect.size.width = SCREEN_RIGHT;
	screen_rect.size.height = SCREEN_BOTTOM;

	// show warning if there is no way to save
	if (eeprom_present() == EEPROM_NONE) {
		next_screen = SCREEN_NOSAVE;
	} else {
		game_save = save_read();
		if (game_save.check == 0) {
			game_save.check = 1;
			save_write(game_save);
		}
	}
}

void change_screen(ScreenType next_screen) {
	if (next_screen == screen_current)
		return;

	mem_zone_free_all(&memory_pool);

	switch (next_screen) {
		case SCREEN_NOSAVE:
			screen_create = &no_save_screen_create;
			screen_tick = &no_save_screen_tick;
			screen_display = &no_save_screen_display;
			break;
		case SCREEN_MAIN:
			screen_create = &main_screen_create;
			screen_tick = &main_screen_tick;
			screen_display = &main_screen_display;
			break;
		case SCREEN_MAIN_MENU:
			screen_create = &main_menu_screen_create;
			screen_tick = &main_menu_screen_tick;
			screen_display = &main_menu_screen_display;
			break;
		case SCREEN_INFINITE_MENU:
			screen_create = &infinite_menu_screen_create;
			screen_tick = &infinite_menu_screen_tick;
			screen_display = &infinite_menu_screen_display;
			break;
		case SCREEN_MINIGAME_DETAIL:
			screen_create = &minigame_detail_screen_create;
			screen_tick = &minigame_detail_screen_tick;
			screen_display = &minigame_detail_screen_display;
			break;
		case SCREEN_MINIGAME_PLAY:
			screen_create = &minigame_play_screen_create;
			screen_tick = &minigame_play_screen_tick;
			screen_display = &minigame_play_screen_display;
			break;
		default:
			abort();
	}

	screen_current = next_screen;

	screen_create();
}
