#include "minigame_play_screen.h"

#include <libdragon.h>

#include "../definitions.h"
#include "../minigames.h"
#include "../utils/mem_pool.h"

extern struct mem_zone memory_pool;
extern MiniGame selected_minigame;

void minigame_play_screen_create() {

}

ScreenType minigame_play_screen_tick(struct controller_data* keys_held, struct controller_data* keys_up, int connected_controllers) {
    for (int i = 0; i < 4; ++i) {
        if ((*keys_up).c[i].start)
            return SCREEN_MINIGAME_DETAIL;
    }

    return SCREEN_MINIGAME_PLAY;
}

void minigame_play_screen_display(display_context_t disp) {
    graphics_fill_screen(disp, BLACK);

    /* Set the text output color */
    graphics_set_color(GRAY, BLACK);
    graphics_draw_text(disp, 0, RES_Y - 20, "Press START to exit");
}

void minigame_play_screen_destroy() {

}
