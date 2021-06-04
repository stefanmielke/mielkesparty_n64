#include "minigame_detail_screen.h"

#include <libdragon.h>

#include "../definitions.h"
#include "../minigames.h"
#include "../utils/mem_pool.h"

extern struct mem_zone memory_pool;
extern MiniGame selected_minigame;

const char* get_minigame_name();

void minigame_detail_screen_create() {

}

ScreenType minigame_detail_screen_tick(struct controller_data* keys_held, struct controller_data* keys_up, int connected_controllers) {
    for (int i = 0; i < 4; ++i) {
        if ((*keys_up).c[i].B)
            return SCREEN_INFINITE_MENU;

        if ((*keys_up).c[i].start)
            return SCREEN_MINIGAME_PLAY;
    }

    return SCREEN_MINIGAME_DETAIL;
}

void minigame_detail_screen_display(display_context_t disp) {
    /* Set the text output color */
    graphics_set_color(BLUE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) - 20, get_minigame_name());

    graphics_set_color(WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 60, (RES_Y / 2) + 20, "Press START to begin");
    graphics_set_color(GRAY, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 55, (RES_Y / 2) + 50, "Press B to go back");
}

const char* get_minigame_name() {
    switch (selected_minigame) {
        case MINIGAME_FLYINGBATS:
            return "Flying Bats";
        default:
            return "No MiniGame selected";
    }
}
