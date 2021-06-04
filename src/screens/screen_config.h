#pragma once

#include <libdragon.h>

typedef enum screen_type {
    SCREEN_NONE,
    SCREEN_NOSAVE,
    SCREEN_MAIN,
    SCREEN_MAIN_MENU,
    SCREEN_INFINITE_MENU,
    SCREEN_MINIGAME_DETAIL,
    SCREEN_MINIGAME_PLAY
} ScreenType;

typedef void (*fnCreate)();
/** Returns next screen (same if shouldn't change) */
typedef ScreenType (*fnTick)();
typedef void (*fnDisplay)(display_context_t disp);
