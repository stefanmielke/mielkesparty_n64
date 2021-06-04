#include "main_menu_screen.h"

#include <libdragon.h>

#include "../definitions.h"
#include "../utils/mem_pool.h"

typedef struct main_menu_screen {
    char current_menu_item;
} MainMenuScreen;

MainMenuScreen* screen;
#define max_menu_items 4

void main_menu_screen_create(struct mem_zone memory_pool) {
    screen = mem_zone_alloc(&memory_pool, sizeof(MainMenuScreen));
    screen->current_menu_item = 0;
}

ScreenType main_menu_screen_tick(struct controller_data* keys_held, struct controller_data* keys_up, int connected_controllers) {
    // checking pressing buttons before moving the cursor
    for (int i = 0; i < 4; ++i) {
        if ((*keys_up).c[i].B)
            return SCREEN_MAIN;

        if ((*keys_up).c[i].A || (*keys_up).c[i].start)
            switch (screen->current_menu_item)
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
            --screen->current_menu_item;

        if ((*keys_up).c[i].down)
            ++screen->current_menu_item;
    }

    if (screen->current_menu_item < 0)
        screen->current_menu_item = max_menu_items - 1;
    if (screen->current_menu_item >= max_menu_items)
        screen->current_menu_item = 0;

    return SCREEN_MAIN_MENU;
}

void main_menu_screen_display(display_context_t disp) {
    graphics_fill_screen(disp, BLACK);

    /* Set the text output color */
    graphics_set_color(BLUE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) - 20, "Main Menu");

    graphics_set_color(screen->current_menu_item == 0 ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) + 20, "Infinite");
    graphics_set_color(screen->current_menu_item == 1 ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) + 30, "Score");
    graphics_set_color(screen->current_menu_item == 2 ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) + 40, "Multiplayer");
    graphics_set_color(screen->current_menu_item == 3 ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) + 50, "Back");
}

void main_menu_screen_destroy() {

}