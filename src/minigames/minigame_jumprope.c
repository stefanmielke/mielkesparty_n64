#include "minigame_jumprope.h"

#include "minigame_defs.h"
#include "../../libs/libdragon-extensions/include/rect.h"
#include "../../libs/libdragon-extensions/include/mem_pool.h"
#include "../../libs/libdragon-extensions/include/easing.h"
#include "../../libs/libdragon-extensions/include/tween.h"

#define ROPE_SPEED_INIT 1000.f
#define ROPE_SPEED_DEC 10.f
#define ROPE_SPEED_MIN 400.f
#define ROPE_SPEED_CHANGE_TIME 1

#define GRAVITY .3f
#define GROUND_Y 200.f
#define JUMP_FORCE 6
#define MAX_SPEED_UP 20
#define MAX_SPEED_DOWN 6

typedef struct {
	Rect rect;
	Position speed;
	bool jumped;
	bool alive;
	bool isPlaying;
	// AnimatedSprite *anim;
} JP_PlayerData;

typedef enum { JP_START, JP_PLAYING, JP_DYING, JP_DEAD } JP_State;

typedef struct {
	JP_State state;
	bool wants_to_save;
	bool saving;
	bool is_new_record;
	size_t seconds_record;
	size_t seconds;
	timer_link_t *time_timer;
	timer_link_t *post_start_timer;
	timer_link_t *post_death_timer;
	JP_PlayerData players[4];
	float current_rope_time;
	size_t current_rope_counter;
	Position current_rope_position;
	float current_start_rope_position_y;
	sprite_t *sprites;
	Tween *tween;
	Tween *start_rope_tween;
} JP_MiniGameData;

JP_MiniGameData *jp_data;
char *jp_time_string;	 // format mm:ss\0
char *jp_record_string;	 // format mm:ss\0

void jp_player_die(int player_id);
void jp_set_time();
void jp_rope_tween(void *target_object, float current_value);
void jp_start_rope_tween(void *target_object, float current_value);
void jp_rope_tween_end_callback(void *target_object);

void jp_post_start(int ovfl);
void jp_post_death(int ovfl);
void jp_time_inc(int ovfl);

void minigame_jumprope_create() {
	jp_data = mem_zone_alloc(&memory_pool, sizeof(JP_MiniGameData));
	jp_data->state = JP_START;
	jp_data->seconds = 0;
	jp_data->saving = false;
	jp_data->wants_to_save = false;
	jp_data->is_new_record = false;
	jp_data->seconds_record = game_save.body.times[MINIGAME_JUMPROPE - 1];

	jp_time_string = mem_zone_alloc(&memory_pool, sizeof(char) * 9);  // format mm:ss\0
	jp_set_time();

	jp_record_string = mem_zone_alloc(&memory_pool, sizeof(char) * 9);	// format - mm:ss\0
	char record_minutes = jp_data->seconds_record / 60;
	char record_seconds = jp_data->seconds_record % 60;
	snprintf(jp_record_string, 8, "%02d:%02d", record_minutes, record_seconds);

	for (size_t i = 0; i < 4; ++i) {
		jp_data->players[i].rect.pos.x = 77 + SCREEN_BORDER + (i * 38);
		jp_data->players[i].rect.pos.y = 200;
		jp_data->players[i].rect.size.width = 32;
		jp_data->players[i].rect.size.height = 32;

		jp_data->players[i].isPlaying = players_ready[i];
		jp_data->players[i].alive = players_ready[i];

		// if (jp_data->players[i].isPlaying) {
		// 	jp_data->players[i].anim = animated_sprite_init(&memory_pool, jp_data->sprites,
		// 													new_size_same(11), new_position(11, 13),
		// 													SPRITE_fb_bat_1, SPRITE_fb_bat_4, 100);
		// }
	}

	jp_data->current_rope_time = ROPE_SPEED_INIT;
	jp_data->current_rope_position.x = 75 + SCREEN_BORDER;
	jp_data->current_rope_position.y = 100;
	jp_data->current_rope_counter = 0;
	jp_data->current_start_rope_position_y = 150;

	audio_load_and_play_bgm(audio_player, BGM_JUMP_ROPE, "sfx/intro.raw");

	jp_data->tween = tween_init(&memory_pool);
	tween_start(jp_data->tween, NULL, &easing_cubic_in_out, jp_data->current_rope_time,
				&jp_rope_tween_end_callback, true, true);
	tween_set_to_float(jp_data->tween, 100, 200, &jp_rope_tween);

	jp_data->start_rope_tween = tween_init(&memory_pool);
	tween_start(jp_data->start_rope_tween, NULL, &easing_cubic_in_out, jp_data->current_rope_time,
				NULL, true, true);
	tween_set_to_float(jp_data->start_rope_tween, 125, 175, &jp_start_rope_tween);

	jp_data->time_timer = new_timer(TIMER_TICKS(SECOND), TF_CONTINUOUS, jp_time_inc);
	jp_data->post_start_timer = new_timer(TIMER_TICKS(1 * SECOND), TF_ONE_SHOT, jp_post_start);
}

bool minigame_jumprope_tick() {
	if (jp_data->saving) {
		save_write(&game_save);

		jp_data->saving = false;
		jp_data->wants_to_save = false;
	}
	if (jp_data->wants_to_save) {
		jp_data->saving = true;
	}

	if (jp_data->state == JP_DEAD || jp_data->state == JP_DYING)
		return true;

	tween_tick(jp_data->tween);
	tween_tick(jp_data->start_rope_tween);

	for (size_t i = 0; i < 4; ++i) {
		JP_PlayerData *p = &jp_data->players[i];
		if (!p->alive)
			continue;

		if (p->rect.pos.y < GROUND_Y)
			p->speed.y += GRAVITY;

		if (p->speed.y < -MAX_SPEED_UP)
			p->speed.y = -MAX_SPEED_UP;
		else if (p->speed.y > MAX_SPEED_DOWN)
			p->speed.y = MAX_SPEED_DOWN;

		p->rect.pos.y += p->speed.y;

		if (p->rect.pos.y >= GROUND_Y) {
			p->speed.y = 0;
			p->rect.pos.y = GROUND_Y;
		}

		if (connected_controllers[i]) {
			if (keys_held.c[i].A && p->rect.pos.y == GROUND_Y) {
				p->speed.y = -JUMP_FORCE;
			}
		}

		if (p->rect.pos.y == GROUND_Y && jp_data->current_rope_position.y >= GROUND_Y - 0.1f &&
			jp_data->current_rope_position.y <= GROUND_Y + 0.1f) {
			jp_player_die(i);
		}
	}

	return true;
}

void minigame_jumprope_display(display_context_t disp) {
	if (jp_data->state == JP_DEAD) {
		graphics_set_color(RED, BLACK);
		graphics_draw_text(disp, (RES_X / 2) - 20, (RES_Y / 2) - 20, "-FINISH-");
		graphics_set_color(WHITE, BLACK);
		graphics_draw_text(disp, (RES_X / 2) - 60, (RES_Y / 2) + 10, "Total Time:");
		graphics_set_color(jp_data->is_new_record ? GREEN : WHITE, BLACK);
		graphics_draw_text(disp, (RES_X / 2) + 30, (RES_Y / 2) + 10, jp_time_string);

		if (jp_data->wants_to_save) {
			graphics_set_color(RED, BLACK);
			graphics_draw_text(disp, (RES_X / 2) - 30, SCREEN_BOTTOM - 20, "SAVING NEW TIME...");
		}
		return;
	}

	const int end_rope_pos_x = jp_data->current_rope_position.x + 150;

	if (jp_data->tween->is_reversing) {
		graphics_draw_line(disp, jp_data->current_rope_position.x, jp_data->current_rope_position.y,
						   end_rope_pos_x, jp_data->current_rope_position.y, GRAY);

		graphics_draw_line(disp, SCREEN_LEFT + 30, jp_data->current_start_rope_position_y,
						   jp_data->current_rope_position.x, jp_data->current_rope_position.y,
						   GRAY);
		graphics_draw_line(disp, SCREEN_RIGHT - 30, jp_data->current_start_rope_position_y,
						   end_rope_pos_x, jp_data->current_rope_position.y, GRAY);
	}
	for (size_t i = 0; i < 4; ++i) {
		if (!jp_data->players[i].alive)
			continue;

		JP_PlayerData *p = &jp_data->players[i];
		graphics_draw_box(disp, p->rect.pos.x, p->rect.pos.y - p->rect.size.height,
						  p->rect.size.width, p->rect.size.height, GREEN);
	}
	if (!jp_data->tween->is_reversing) {
		graphics_draw_line(disp, jp_data->current_rope_position.x, jp_data->current_rope_position.y,
						   end_rope_pos_x, jp_data->current_rope_position.y, WHITE);

		graphics_draw_line(disp, SCREEN_LEFT + 30, jp_data->current_start_rope_position_y,
						   jp_data->current_rope_position.x, jp_data->current_rope_position.y,
						   WHITE);
		graphics_draw_line(disp, SCREEN_RIGHT - 30, jp_data->current_start_rope_position_y,
						   end_rope_pos_x, jp_data->current_rope_position.y, WHITE);
	}

	graphics_set_color(WHITE, TRANSP);
	graphics_draw_text(disp, SCREEN_LEFT, SCREEN_TOP, jp_time_string);
	graphics_set_color(GRAY, TRANSP);
	graphics_draw_text(disp, SCREEN_LEFT + 45, SCREEN_TOP, jp_record_string);
}

void minigame_jumprope_destroy() {
}

void jp_player_die(int player_id) {
	jp_data->players[player_id].alive = false;

	bool is_any_alive = false;
	for (size_t i = 0; i < 4; ++i) {
		if (jp_data->players[i].alive) {
			is_any_alive = true;
			break;
		}
	}

	if (!is_any_alive) {
		stop_timer(jp_data->time_timer);

		// check if record
		if (jp_data->seconds > jp_data->seconds_record) {
			jp_data->is_new_record = true;
			game_save.body.times[MINIGAME_JUMPROPE - 1] = jp_data->seconds;
		}

		jp_data->state = JP_DYING;
		jp_data->post_death_timer = new_timer(TIMER_TICKS(1 * SECOND), TF_ONE_SHOT, jp_post_death);
	}
}

void jp_set_time() {
	char minutes = jp_data->seconds / 60;
	char seconds = jp_data->seconds % 60;
	snprintf(jp_time_string, 8, "%02d:%02d", minutes, seconds);
}

void jp_rope_tween(void *target_object, float current_value) {
	jp_data->current_rope_position.y = current_value;
}

void jp_start_rope_tween(void *target_object, float current_value) {
	jp_data->current_start_rope_position_y = current_value;
}

void jp_rope_tween_end_callback(void *target_object) {
	if (jp_data->current_rope_time <= ROPE_SPEED_MIN)
		return;

	jp_data->current_rope_counter++;

	if (jp_data->current_rope_counter >= ROPE_SPEED_CHANGE_TIME) {
		jp_data->current_rope_counter = 0;
		jp_data->current_rope_time -= ROPE_SPEED_DEC;
		tween_change_duration(jp_data->tween, jp_data->current_rope_time);
		tween_change_duration(jp_data->start_rope_tween, jp_data->current_rope_time);
	}
}

void jp_post_start(int ovfl) {
	jp_data->state = JP_PLAYING;
}

void jp_post_death(int ovfl) {
	jp_data->state = JP_DEAD;
	jp_data->wants_to_save = true;
}

void jp_time_inc(int ovfl) {
	++jp_data->seconds;
	jp_set_time();
}
