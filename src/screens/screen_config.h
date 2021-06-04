#pragma once

#include <libdragon.h>

typedef enum screen_type {
    SCREEN_NONE,
    SCREEN_MAIN,
    SCREEN_MAIN_MENU
} ScreenType;

typedef void (*fnCreate)();
/** Returns next screen (same if shouldn't change) */
typedef ScreenType (*fnTick)(struct controller_data* keys_held, struct controller_data* keys_up, int connected_controllers);
typedef void (*fnDisplay)(display_context_t disp);
typedef void (*fnDestroy)();
