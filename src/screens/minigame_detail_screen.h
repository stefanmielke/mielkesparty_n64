#pragma once

#include "screen_config.h"

void minigame_detail_screen_create();
ScreenType minigame_detail_screen_tick(struct controller_data* keys_held, struct controller_data* keys_up, int connected_controllers);
void minigame_detail_screen_display(display_context_t disp);
