#include "main_screen.h"

#include <libdragon.h>

#include "../definitions.h"
#include "../utils/mem_pool.h"

void main_screen_create() {
    
}

ScreenType main_screen_tick(struct controller_data* keys_held, struct controller_data* keys_up, int connected_controllers) {
    for (int i = 0; i < 4; ++i) {
        if ((*keys_up).c[i].start)
            return SCREEN_MAIN_MENU;
    }

    return SCREEN_MAIN;
}

void main_screen_display(display_context_t disp) {
    graphics_fill_screen(disp, BLACK);

    /* Set the text output color */
    graphics_set_color(BLUE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 55, (RES_Y / 2) - 20, "Mielke's Party");

    graphics_set_color(RED, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 45, (RES_Y / 2) + 20, "Press START");
}
