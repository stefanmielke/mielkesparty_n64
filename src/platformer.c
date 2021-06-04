#include <libdragon.h>

#include "definitions.h"
#include "minigames.h"
#include "utils/mem_pool.h"
#include "screens/screen_config.h"
#include "screens/main_screen.h"
#include "screens/main_menu_screen.h"
#include "screens/infinite_menu_screen.h"
#include "screens/minigame_detail_screen.h"
#include "screens/minigame_play_screen.h"

ScreenType screen_current = SCREEN_NONE;
ScreenType next_screen = SCREEN_MAIN;

fnCreate screen_create;
fnTick screen_tick;
fnDisplay screen_display;

struct controller_data keys_held;
struct controller_data keys_released;
int connected_controllers;

struct mem_zone memory_pool;

MiniGame selected_minigame = MINIGAME_NONE;

void change_screen(ScreenType next_screen);
void setup();

int main() {
    setup();

    /* Main loop test */
    while(1) {
        if (next_screen != SCREEN_NONE && screen_current != next_screen)
            change_screen(next_screen);

        controller_scan();
        keys_held = get_keys_held();
        keys_released = get_keys_up();
        connected_controllers = get_controllers_present();

        next_screen = screen_tick(&keys_held, &keys_released, connected_controllers);

        /* Grab a render buffer */
        static display_context_t disp = 0;
        while( !(disp = display_lock()) );
        
        graphics_fill_screen(disp, BLACK);

        screen_display(disp);

        if (next_screen != SCREEN_NONE && screen_current != next_screen) {
            graphics_set_color( 0xFFFFFFFF, 0x0 );
            graphics_draw_text(disp, 0, (RES_Y) - 20, "Loading next screen...");
        }

        display_show(disp);
    }
}

void setup() {
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    display_init( SCREEN_RESOLUTION, DEPTH_16_BPP, NUM_BUFFERS, GAMMA_NONE, ANTIALIAS_OFF );
    rdp_init();
    controller_init();
    timer_init();

    mem_zone_init(&memory_pool, 1 * 1024);

    WHITE = graphics_make_color(255, 255, 255, 255);
    BLACK = graphics_make_color(0, 0, 0, 255);
    RED = graphics_make_color(255, 0, 0, 255);
    GREEN = graphics_make_color(0, 255, 0, 255);
    BLUE = graphics_make_color(0, 0, 255, 255);
    GRAY = graphics_make_color(60, 60, 60, 255);
}

void change_screen(ScreenType next_screen) {
    if (next_screen == screen_current)
        return;

    mem_zone_free_all(&memory_pool);

    switch (next_screen)
    {
    case SCREEN_MAIN:
        screen_create = &main_screen_create;
        screen_tick = &main_screen_tick;
        screen_display = &main_screen_display;
        break;
    case SCREEN_MAIN_MENU:
        screen_create = &main_menu_screen_create;
        screen_tick = &main_menu_screen_tick;
        screen_display = &main_menu_screen_display;
        break;
    case SCREEN_INFINITE_MENU:
        screen_create = &infinite_menu_screen_create;
        screen_tick = &infinite_menu_screen_tick;
        screen_display = &infinite_menu_screen_display;
        break;
    case SCREEN_MINIGAME_DETAIL:
        screen_create = &minigame_detail_screen_create;
        screen_tick = &minigame_detail_screen_tick;
        screen_display = &minigame_detail_screen_display;
        break;
    case SCREEN_MINIGAME_PLAY:
        screen_create = &minigame_play_screen_create;
        screen_tick = &minigame_play_screen_tick;
        screen_display = &minigame_play_screen_display;
        break;
    default:
        abort();
    }

    screen_current = next_screen;

    screen_create();
}
