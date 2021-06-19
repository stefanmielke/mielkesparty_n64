#pragma once

#include "minigames.h"
#include "screens/screen_config.h"
#include "utils/audio.h"
#include "utils/save.h"
#include "../libs/libdragon-extensions/include/mem_pool.h"
#include "../libs/libdragon-extensions/include/rect.h"
#include "../libs/libdragon-extensions/include/scene_manager.h"

Rect screen_rect;

SceneManager *scene_manager;

struct controller_data keys_held;
struct controller_data keys_released;
bool connected_controllers[4];

MemZone memory_pool;
sprite_t *ui_sprites;
Audio *audio_player;

MiniGame selected_minigame;
bool players_ready[4];

SaveFile game_save;
