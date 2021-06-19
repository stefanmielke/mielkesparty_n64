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

void change_screen(short curr_screen, short next_screen);
void setup();

int main() {
	setup();

	while (1) {
		// tick controller
		controller_scan();
		keys_held = get_keys_held();
		keys_released = get_keys_up();
		int controllers_present = get_controllers_present();
		connected_controllers[0] = controllers_present & CONTROLLER_1_INSERTED;
		connected_controllers[1] = controllers_present & CONTROLLER_2_INSERTED;
		connected_controllers[2] = controllers_present & CONTROLLER_3_INSERTED;
		connected_controllers[3] = controllers_present & CONTROLLER_4_INSERTED;

		scene_manager_tick(scene_manager);

		audio_tick(audio_player);

		// tick render
		static display_context_t disp = 0;
		while (!(disp = display_lock()))
			;

		if (scene_manager->current_scene_id != scene_manager->next_scene_id) {
			graphics_fill_screen(disp, BLACK);
			graphics_set_color(WHITE, TRANSP);
			graphics_draw_text(disp, SCREEN_BORDER, RES_Y - SCREEN_BORDER - 20, "Loading...");
		} else {
			graphics_fill_screen(disp, BLACK);
			scene_manager_display(scene_manager, disp);
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

	mem_zone_init(&memory_pool, 1 * 1024 * 1024);

	scene_manager = scene_manager_init(NULL, &memory_pool, &change_screen);

	alloc_and_load_spritesheet_interface(ui_sprites);
	audio_player = audio_setup(AUDIO_FREQUENCY);
	audio_current_sfx_set = SfxSet_None;

	// const char *sfx_files[6] = {"/sfx/back.raw",
	// 							"/sfx/click.raw",
	// 							"/sfx/confirm.raw",
	// 							"/sfx/controller_connected.raw",
	// 							"/sfx/controller_unconnected.raw",
	// 							"/sfx/unconfirm.raw"};
	// audio_load_sfx(audio_player, sfx_files, 6);

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
	screen_rect.size.width = SCREEN_RIGHT - SCREEN_LEFT;
	screen_rect.size.height = SCREEN_BOTTOM - SCREEN_TOP;

	// show warning if there is no way to save
	if (eeprom_present() == EEPROM_NONE) {
		scene_manager_change_scene(scene_manager, SCREEN_NOSAVE);
	} else {
		game_save = save_read();
		save_write(&game_save);
		scene_manager_change_scene(scene_manager, SCREEN_MAIN);
	}
}

void change_screen(short curr_screen, short next_screen) {
	switch (next_screen) {
		case SCREEN_NOSAVE:
			scene_manager_set_callbacks(scene_manager, NULL, &no_save_screen_tick,
										&no_save_screen_display, NULL);
			break;
		case SCREEN_MAIN:
			scene_manager_set_callbacks(scene_manager, &main_screen_create, &main_screen_tick,
										&main_screen_display, NULL);
			break;
		case SCREEN_MAIN_MENU:
			scene_manager_set_callbacks(scene_manager, &main_menu_screen_create,
										&main_menu_screen_tick, &main_menu_screen_display, NULL);
			break;
		case SCREEN_INFINITE_MENU:
			scene_manager_set_callbacks(scene_manager, &infinite_menu_screen_create,
										&infinite_menu_screen_tick, &infinite_menu_screen_display,
										NULL);
			break;
		case SCREEN_MINIGAME_DETAIL:
			scene_manager_set_callbacks(scene_manager, &minigame_detail_screen_create,
										&minigame_detail_screen_tick,
										&minigame_detail_screen_display, NULL);
			break;
		case SCREEN_MINIGAME_PLAY:
			scene_manager_set_callbacks(scene_manager, &minigame_play_screen_create,
										&minigame_play_screen_tick, &minigame_play_screen_display,
										NULL);
			break;
		default:
			abort();
	}
}

void audio_load_sfx_set(uint8_t set) {
	if (audio_current_sfx_set == set)
		return;

	audio_unload_all_sfx(audio_player);
	audio_current_sfx_set = set;

	const char **audio_set = NULL;
	switch (set) {
		case SfxSet_Menu:
			audio_set = calloc(sizeof(const char *), 6);
			audio_set[0] = "/sfx/back.raw";
			audio_set[1] = "/sfx/click.raw";
			audio_set[2] = "/sfx/confirm.raw";
			audio_set[3] = "/sfx/controller_connected.raw";
			audio_set[4] = "/sfx/controller_unconnected.raw";
			audio_set[5] = "/sfx/unconfirm.raw";
			break;
		default:
			break;
	}

	if (audio_set) {
		audio_load_sfx(audio_player, audio_set, 6);
		free(audio_set);
	}
}
