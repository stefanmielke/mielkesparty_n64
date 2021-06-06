#include "infinite_menu_screen.h"

#include "../../libs/libdragon/include/libdragon.h"

#include "../definitions.h"
#include "screen_defs.h"
#include "menu_screen.h"
#include "../minigames.h"
#include "../utils/mem_pool.h"

enum menu_items {
    IM_FlyingBats,
    IM_Back,
    IM_MaxItems
};

void infinite_menu_screen_create() {
    menu_screen_create(IM_MaxItems);
}

ScreenType infinite_menu_screen_tick() {
    // checking pressing buttons before moving the cursor
    for (int i = 0; i < 4; ++i) {
        if (keys_released.c[i].B) {
            menu_screen_destroy();
            return SCREEN_MAIN_MENU;
        }

        if (keys_released.c[i].A || keys_released.c[i].start)
            switch (menu_screen->currentMenuItem)
            {
            case IM_FlyingBats:
                menu_screen_destroy();
                selected_minigame = MINIGAME_FLYINGBATS;
                return SCREEN_MINIGAME_DETAIL;
            case IM_Back:
                menu_screen_destroy();
                return SCREEN_MAIN_MENU;
            }
    }

    menu_screen_end_tick();

    return SCREEN_INFINITE_MENU;
}

void infinite_menu_screen_display(display_context_t disp) {
    /* Set the text output color */
    graphics_set_color(BLUE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 70, (RES_Y / 2) - 40, "Infinite Mini-Games");

    graphics_set_color(menu_screen->currentMenuItem == IM_FlyingBats ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 55, (RES_Y / 2), "Flying Bats");
    graphics_set_color(menu_screen->currentMenuItem == IM_Back ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 55, (RES_Y / 2) + 20, "Back");
}
