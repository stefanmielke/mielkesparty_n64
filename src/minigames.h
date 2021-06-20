#pragma once

#include <libdragon.h>

typedef enum minigame {
	MINIGAME_NONE,
	MINIGAME_FLYINGBATS,
	MINIGAME_JUMPROPE,
	MINIGAME_MAX
} MiniGame;

typedef void (*fnGameCreate)();
/** Returns if it should continue */
typedef bool (*fnGameTick)();
typedef void (*fnGameDisplay)(display_context_t disp);
typedef void (*fnGameDestroy)();
