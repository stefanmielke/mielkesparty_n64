#include "infinite_menu_screen.h"

#include <libdragon.h>

#include "../definitions.h"
#include "screen_defs.h"
#include "../minigames.h"
#include "../utils/mem_pool.h"

extern MemZone memory_pool;
extern MiniGame selected_minigame;

typedef struct infinite_menu_screen {
    char current_menu_item;
} InfiniteMenuScreen;

InfiniteMenuScreen* screen;

enum menu_items {
    FlyingBats,
    Back,
    MaxItems
};

void infinite_menu_screen_create() {
    screen = mem_zone_alloc(&memory_pool, sizeof(InfiniteMenuScreen));
    screen->current_menu_item = 0;
}

ScreenType infinite_menu_screen_tick() {
    // checking pressing buttons before moving the cursor
    for (int i = 0; i < 4; ++i) {
        if (keys_released.c[i].B)
            return SCREEN_MAIN_MENU;

        if (keys_released.c[i].A || keys_released.c[i].start)
            switch (screen->current_menu_item)
            {
            case FlyingBats:
                selected_minigame = MINIGAME_FLYINGBATS;
                return SCREEN_MINIGAME_DETAIL;
            case Back:
                return SCREEN_MAIN_MENU;
            }
    }
    for (int i = 0; i < 4; ++i) {
        if (keys_released.c[i].up)
            --screen->current_menu_item;

        if (keys_released.c[i].down)
            ++screen->current_menu_item;
    }

    if (screen->current_menu_item < 0)
        screen->current_menu_item = MaxItems - 1;
    if (screen->current_menu_item >= MaxItems)
        screen->current_menu_item = 0;

    return SCREEN_INFINITE_MENU;
}

void infinite_menu_screen_display(display_context_t disp) {
    /* Set the text output color */
    graphics_set_color(BLUE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 70, (RES_Y / 2) - 40, "Infinite Mini-Games");

    graphics_set_color(screen->current_menu_item == FlyingBats ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 55, (RES_Y / 2), "Flying Bats");
    graphics_set_color(screen->current_menu_item == Back ? RED : WHITE, BLACK);
    graphics_draw_text(disp, (RES_X / 2) - 55, (RES_Y / 2) + 20, "Back");
}
