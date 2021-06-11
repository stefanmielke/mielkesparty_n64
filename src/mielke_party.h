#pragma once

#include "minigames.h"
#include "utils/audio.h"
#include "../libs/libdragon-extensions/include/mem_pool.h"
#include "../libs/libdragon-extensions/include/util_defs.h"
#include "screens/screen_config.h"

ScreenType screen_current;
ScreenType next_screen;
Rect screen_rect;

fnCreate screen_create;
fnTick screen_tick;
fnDisplay screen_display;

struct controller_data keys_held;
struct controller_data keys_released;
bool connected_controllers[4];

MemZone memory_pool;
sprite_t *ui_sprites;
audio_t *audio_player;

MiniGame selected_minigame;
bool players_ready[4];

SaveFile game_save;
void save_write(SaveFile save);
SaveFile save_read();
