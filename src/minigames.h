#pragma once

typedef enum minigame {
    MINIGAME_NONE,
    MINIGAME_FLYINGBATS
} MiniGame;

typedef void (*fnGameCreate)();
/** Returns if it should continue */
typedef bool (*fnGameTick)();
typedef void (*fnGameDisplay)(display_context_t disp);
