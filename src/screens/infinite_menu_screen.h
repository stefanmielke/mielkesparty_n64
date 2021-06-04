#pragma once

#include "screen_config.h"

void infinite_menu_screen_create();
ScreenType infinite_menu_screen_tick(struct controller_data* keys_held, struct controller_data* keys_up, int connected_controllers);
void infinite_menu_screen_display(display_context_t disp);
