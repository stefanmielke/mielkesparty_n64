#pragma once

#include "screen_config.h"

void main_menu_screen_create();
ScreenType main_menu_screen_tick(struct controller_data* keys_held, struct controller_data* keys_up, int connected_controllers);
void main_menu_screen_display(display_context_t disp);
void main_menu_screen_destroy();
