#include "minigame_play_screen.h"

#include <libdragon.h>

#include "../definitions.h"
#include "../minigames.h"
#include "../utils/mem_pool.h"

#include "../minigames/minigame_flyingbats.h"

extern struct mem_zone memory_pool;
extern MiniGame selected_minigame;

typedef struct minigame_screen {
    fnGameCreate create;
    fnGameTick tick;
    fnGameDisplay display;
} MiniGameScreen;

MiniGameScreen* data;

void minigame_play_screen_create() {
    data = mem_zone_alloc(&memory_pool, sizeof(MiniGameScreen));

    switch (selected_minigame) {
        case MINIGAME_FLYINGBATS:
            data->create = &minigame_flyingbats_create;
            data->tick = &minigame_flyingbats_tick;
            data->display = &minigame_flyingbats_display;
            break;
        default:
            abort();
    }

    data->create();
}

ScreenType minigame_play_screen_tick(struct controller_data* keys_held, struct controller_data* keys_up, int connected_controllers) {
    for (int i = 0; i < 4; ++i) {
        if ((*keys_up).c[i].start)
            return SCREEN_MINIGAME_DETAIL;
    }

    data->tick();

    return SCREEN_MINIGAME_PLAY;
}

void minigame_play_screen_display(display_context_t disp) {
    data->display(disp);

    /* Set the text output color */
    graphics_set_color(GRAY, BLACK);
    graphics_draw_text(disp, 0, RES_Y - 20, "Press START to exit");
}
