#pragma once

#include "../utils/mem_pool.h"
#include "../screens/minigame_detail_screen.h"

// game globals
extern MemZone memory_pool;
extern struct controller_data keys_held;
extern struct controller_data keys_released;
extern bool connected_controllers[4];
extern Rect screen_rect;
extern SaveFile game_save;
extern bool players_ready[4];

extern void save_write(SaveFile save);
extern SaveFile save_read();

#define DRAW_RECT(disp, rect, color) (graphics_draw_box(disp, rect.pos.x, rect.pos.y, rect.size.width, rect.size.height, color))