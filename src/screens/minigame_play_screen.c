#include "minigame_play_screen.h"

#include <libdragon.h>

#include "../definitions.h"
#include "screen_defs.h"
#include "../minigames.h"
#include "../utils/mem_pool.h"

#include "../minigames/minigame_flyingbats.h"

#include <stdio.h>

extern MemZone memory_pool;
extern MiniGame selected_minigame;

typedef struct minigame_screen {
    fnGameCreate create;
    fnGameTick tick;
    fnGameDisplay display;
    char* print;
} MiniGameScreen;

MiniGameScreen* play_screen_data;

void minigame_play_screen_create() {
    play_screen_data = mem_zone_alloc(&memory_pool, sizeof(MiniGameScreen));

    switch (selected_minigame) {
        case MINIGAME_FLYINGBATS:
            play_screen_data->create = &minigame_flyingbats_create;
            play_screen_data->tick = &minigame_flyingbats_tick;
            play_screen_data->display = &minigame_flyingbats_display;
            break;
        default:
            abort();
    }

    play_screen_data->create();
}

ScreenType minigame_play_screen_tick() {
    for (int i = 0; i < 4; ++i) {
        if (keys_released.c[i].start)
            return SCREEN_MINIGAME_DETAIL;
    }

    if (!play_screen_data->tick())
        return SCREEN_MINIGAME_DETAIL;

    return SCREEN_MINIGAME_PLAY;
}

void minigame_play_screen_display(display_context_t disp) {
    play_screen_data->display(disp);

    /* Set the text output color */
    graphics_set_color(GRAY, BLACK);
    graphics_draw_text(disp, SCREEN_BORDER, SCREEN_BOTTOM - 20, "START to exit");
}