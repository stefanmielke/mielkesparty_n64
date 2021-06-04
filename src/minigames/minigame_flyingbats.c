#include "minigame_flyingbats.h"

#include "../definitions.h"

void minigame_flyingbats_create() {

}

bool minigame_flyingbats_tick() {
    return true;
}

void minigame_flyingbats_display(display_context_t disp) {
    graphics_set_color(GRAY, BLACK);
    graphics_draw_text(disp, 0, 0, "FLY!");
}
