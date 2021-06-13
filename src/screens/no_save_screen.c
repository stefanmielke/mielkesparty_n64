#include "no_save_screen.h"

#include <libdragon.h>

#include "../definitions.h"
#include "screen_defs.h"

void no_save_screen_create() {
    
}

short no_save_screen_tick() {
    for (int i = 0; i < 4; ++i) {
        if (keys_released.c[i].start)
            return SCREEN_MAIN;
    }

    return SCREEN_NOSAVE;
}

void no_save_screen_display(display_context_t disp) {
    graphics_set_color(RED, BLACK);
    graphics_draw_text(disp, SCREEN_BORDER + 20, SCREEN_TOP + 60, "\tNo SaveSlots detected.\n\n\nTO SOLVE THE ISSUE:\n\n- Edit save_db.txt\n\n- Force at least EEP4K on\n  your flashcart menu to save.\n\n\n\n\tPress START to continue\n\t\twithout saving.");
}
