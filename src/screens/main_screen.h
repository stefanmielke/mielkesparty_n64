#pragma once

#include "screen_config.h"

// void set_main_screen(fnCreate* create, fnTick* tick, fnDisplay* display, fnDestroy* destroy);
void main_screen_create();
void main_screen_tick();
void main_screen_display(display_context_t disp);
void main_screen_destroy();
