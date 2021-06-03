#pragma once

#include <libdragon.h>

typedef void (*fnCreate)();
typedef void (*fnTick)();
typedef void (*fnDisplay)(display_context_t disp);
typedef void (*fnDestroy)();

typedef enum screen_type {
    SCREEN_NONE,
    SCREEN_MAIN
} ScreenType;
