#include "main_menu_screen.h"

#include <libdragon.h>

#include "../definitions.h"
#include "screen_defs.h"
#include "menu_screen.h"
#include "../utils/mem_pool.h"
#include "../gfx_h/gfx_interface.h"

enum menu_items {
    MM_Infinite,
    MM_Score,
    MM_Multiplayer,
    MM_MaxItems
};

void main_menu_screen_create() {
    menu_screen_create(MM_MaxItems);
}

ScreenType main_menu_screen_tick() {
    // checking pressing buttons before moving the cursor
    for (int i = 0; i < 4; ++i) {
        if (keys_released.c[i].B) {
            menu_screen_destroy();
            PLAY_AUDIO(SFX_BACK);
            return SCREEN_MAIN;
        }

        if (keys_released.c[i].A || keys_released.c[i].start)
            switch (menu_screen->currentMenuItem)
            {
            case MM_Infinite:
                PLAY_AUDIO(SFX_CLICK);
                menu_screen_destroy();
                return SCREEN_INFINITE_MENU;
            case MM_Score:
            case MM_Multiplayer:
                break;
            }
    }

    menu_screen_end_tick();

    return SCREEN_MAIN_MENU;
}

void main_menu_screen_display(display_context_t disp) {
    /* Set the text output color */
    graphics_set_color(BLUE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) - 40, "Main Menu");

    graphics_set_color(menu_screen->currentMenuItem == 0 ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2), "Infinite");
    graphics_set_color(menu_screen->currentMenuItem == 1 ? RED : GRAY, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) + 20, "Score");
    graphics_set_color(menu_screen->currentMenuItem == 2 ? RED : GRAY, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 35, (RES_Y / 2) + 40, "Multiplayer");

    DRAW_BACK_BUTTON();
}
