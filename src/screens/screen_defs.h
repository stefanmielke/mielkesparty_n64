#pragma once

#include "../definitions.h"
#include "../utils/audio.h"
#include "../utils/mem_pool.h"

// game globals
extern MemZone memory_pool;
extern struct controller_data keys_held;
extern struct controller_data keys_released;
extern bool connected_controllers[4];
extern SaveFile game_save;
extern MiniGame selected_minigame;
extern bool players_ready[4];
extern sprite_t *ui_sprites;
extern audio_t *ui_sfx;

#define DRAW_BACK_BUTTON() graphics_draw_sprite_trans_stride(disp, SCREEN_LEFT + 1, SCREEN_BOTTOM - 17, ui_sprites, SPRITE_buttonBack)
#define PLAY_AUDIO(audio) audio_play_sfx(ui_sfx, audio)
