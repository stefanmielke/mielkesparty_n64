#include <assert.h>

#include <libdragon.h>

#include "definitions.h"
#include "screens/screen_config.h"
#include "screens/main_screen.h"

ScreenType screen_current;
fnCreate screen_create;
fnTick screen_tick;
fnDisplay screen_display;
fnDestroy screen_destroy;

void change_screen(ScreenType next_screen);

int main( void ) {
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    display_init( SCREEN_RESOLUTION, DEPTH_16_BPP, NUM_BUFFERS, GAMMA_NONE, ANTIALIAS_OFF );
    rdp_init();
    controller_init();
    timer_init();

    // set_main_screen(screen_create, screen_tick, screen_display, screen_destroy);
    change_screen(SCREEN_MAIN);

    /* Main loop test */
    while(1) {
        /* To do initialize routines */
        controller_scan();

        screen_tick();

        /* Grab a render buffer */
        static display_context_t disp = 0;
        while( !(disp = display_lock()) );

        screen_display(disp);

        /* Force backbuffer flip, ie: draw to screen */
        display_show(disp);
    }
}

void change_screen(ScreenType next_screen) {
    if (next_screen == screen_current)
        return;

    if (screen_current != SCREEN_NONE)
    screen_destroy();

    switch (next_screen)
    {
    case SCREEN_MAIN:
        screen_create = &main_screen_create;
        screen_tick = &main_screen_tick;
        screen_display = &main_screen_display;
        screen_destroy = &main_screen_destroy;
        break;
    default:
        assert("No correct screen sent to change_screen");
    }

    screen_current = next_screen;

    screen_create();
}
