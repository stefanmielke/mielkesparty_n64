#include "main_menu_screen.h"

#include <libdragon.h>

#include "../definitions.h"

int current_menu_item = 0;
#define max_menu_items 4

void main_menu_screen_create() {
    current_menu_item = 0;
}

ScreenType main_menu_screen_tick(struct controller_data* keys_held, struct controller_data* keys_up, int connected_controllers) {
    // checking pressing buttons before moving the cursor
    for (int i = 0; i < 4; ++i) {
        if ((*keys_up).c[i].B)
            return SCREEN_MAIN;

        if ((*keys_up).c[i].A || (*keys_up).c[i].start)
            switch (current_menu_item)
            {
            case 0:
            case 1:
            case 2:
                break;
            case max_menu_items - 1:
                return SCREEN_MAIN;
            }
    }
    for (int i = 0; i < 4; ++i) {
        if ((*keys_up).c[i].up)
            --current_menu_item;

        if ((*keys_up).c[i].down)
            ++current_menu_item;
    }

    if (current_menu_item < 0)
        current_menu_item = max_menu_items - 1;
    if (current_menu_item >= max_menu_items)
        current_menu_item = 0;

    return SCREEN_MAIN_MENU;
}

void main_menu_screen_display(display_context_t disp) {
    graphics_fill_screen(disp, BLACK);

    /* Set the text output color */
    graphics_set_color(BLUE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) - 20, "Main Menu");

    graphics_set_color(current_menu_item == 0 ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) + 20, "Infinite");
    graphics_set_color(current_menu_item == 1 ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) + 30, "Score");
    graphics_set_color(current_menu_item == 2 ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) + 40, "Multiplayer");
    graphics_set_color(current_menu_item == 3 ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) + 50, "Back");
}

void main_menu_screen_destroy() {

}