#include "main_screen.h"

#include <libdragon.h>

#include "../definitions.h"

// void set_main_screen(fnCreate* create, fnTick* tick, fnDisplay* display, fnDestroy* destroy) {
//     create = &main_screen_create;
//     tick = &main_screen_tick;
//     display = &main_screen_display;
//     destroy = &main_screen_destroy;
// }

void main_screen_create() {

}

void main_screen_tick() {

}

void main_screen_display(display_context_t disp) {
    /* Fill the screen */
    graphics_fill_screen( disp, 0x0 );

    /* Set the text output color */
    graphics_set_color( 0x111111FF, 0x0 );

    graphics_draw_text(disp, (RES_X / 2) - 55, (RES_Y / 2) - 20, "Mielke's Party");
}

void main_screen_destroy() {

}