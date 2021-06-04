#pragma once

#include "screen_config.h"

void main_screen_create(struct mem_zone memory_pool);
ScreenType main_screen_tick(struct controller_data* keys_held, struct controller_data* keys_up, int connected_controllers);
void main_screen_display(display_context_t disp);
void main_screen_destroy();
