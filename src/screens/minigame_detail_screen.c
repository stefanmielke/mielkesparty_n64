#include "minigame_detail_screen.h"

#include <stdio.h>
#include "../../libs/libdragon/include/libdragon.h"

#include "../definitions.h"
#include "screen_defs.h"
#include "../minigames.h"
#include "../utils/mem_pool.h"

extern MemZone memory_pool;
extern MiniGame selected_minigame;

char* det_sc_time_string;

const char* get_minigame_name();
const char* get_minigame_record();

void minigame_detail_screen_create() {
    det_sc_time_string = mem_zone_alloc(&memory_pool, sizeof(char)*16); // format Record: mm:ss\0
}

ScreenType minigame_detail_screen_tick() {
    for (int i = 0; i < 4; ++i) {
        if (keys_released.c[i].B)
            return SCREEN_INFINITE_MENU;

        if (keys_released.c[i].start)
            return SCREEN_MINIGAME_PLAY;
    }

    return SCREEN_MINIGAME_DETAIL;
}

void minigame_detail_screen_display(display_context_t disp) {
    /* Set the text output color */
    graphics_set_color(BLUE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) - 40, get_minigame_name());
    graphics_set_color(GREEN, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 45, (RES_Y / 2), get_minigame_record());

    graphics_set_color(WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 70, (RES_Y / 2) + 20, "Press START to begin");
    graphics_set_color(GRAY, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 65, (RES_Y / 2) + 70, "Press B to go back");
}

const char* get_minigame_name() {
    switch (selected_minigame) {
        case MINIGAME_FLYINGBATS:
            return "Flying Bats";
        default:
            return "No MiniGame selected";
    }
}

const char* get_minigame_record() {
    switch (selected_minigame) {
        case MINIGAME_FLYINGBATS:
            {
                int time = game_save.times[MINIGAME_FLYINGBATS-1];
                size_t minutes = time / 60;
                size_t seconds = time % 60;
                snprintf(det_sc_time_string, 15, "Record: %02d:%02d", minutes, seconds);
                return det_sc_time_string;
            }
        default:
            return "No Record";
    }
}
