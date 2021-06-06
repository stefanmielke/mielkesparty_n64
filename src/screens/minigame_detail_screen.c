#include "minigame_detail_screen.h"

#include <stdio.h>
#include <libdragon.h>

#include "../definitions.h"
#include "screen_defs.h"
#include "../minigames.h"
#include "../utils/mem_pool.h"

typedef struct minigame_detail_data {
    bool playerReady[4];
} MinigameDetailData;

MinigameDetailData* md_data;

char* det_sc_time_string;
const char* get_minigame_name(int* offset_x);
const char* get_minigame_record();
const char* get_minigame_description();
const char* get_minigame_controls();

void minigame_detail_screen_create() {
    det_sc_time_string = mem_zone_alloc(&memory_pool, sizeof(char)*9); // format Record: mm:ss\0
    md_data = mem_zone_alloc(&memory_pool, sizeof(MinigameDetailData));
    for (size_t i = 0; i < 4; ++i) {
        md_data->playerReady[i] = false;
    }
}

ScreenType minigame_detail_screen_tick() {
    for (int i = 0; i < 4; ++i) {
        if (md_data->playerReady[i]) {
            if (keys_released.c[i].start)
                return SCREEN_MINIGAME_PLAY;
            if (keys_released.c[i].B)
                md_data->playerReady[i] = false;
        }
        else {
            if (keys_released.c[i].A)
                md_data->playerReady[i] = true;
            if (keys_released.c[i].B)
                return SCREEN_INFINITE_MENU;
        }
    }

    return SCREEN_MINIGAME_DETAIL;
}

void minigame_detail_screen_display(display_context_t disp) {
    /* Set the text output color */
    graphics_set_color(BLUE, BLACK);

    int offset_x;
    const char* name = get_minigame_name(&offset_x);
    graphics_draw_text(disp, SCREEN_LEFT + offset_x, SCREEN_TOP + 5, name);
    graphics_set_color(GREEN, BLACK);
    graphics_draw_text(disp, RES_X / 2 - 4, SCREEN_TOP + 20, get_minigame_record());
    // todo: draw minigame image/video
    graphics_set_color(WHITE, BLACK);
    graphics_draw_text(disp, SCREEN_LEFT, RES_Y / 2 + 20, get_minigame_description());

    graphics_draw_line(disp, RES_X / 2 + 46, SCREEN_TOP, RES_X / 2 + 46, SCREEN_BOTTOM, GRAY);

    graphics_draw_text(disp, RES_X / 2 + 52, SCREEN_TOP + 30, get_minigame_controls());

    if (md_data->playerReady[0]) {
        graphics_set_color(GREEN, BLACK);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 80, "PLAYER 1 (B)");
    }
    else {
        graphics_set_color(GRAY, BLACK);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 80, "PLAYER 1 (A)");
    }
    if (md_data->playerReady[1]) {
        graphics_set_color(GREEN, BLACK);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 65, "PLAYER 2 (B)");
    }
    else {
        graphics_set_color(GRAY, BLACK);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 65, "PLAYER 2 (A)");
    }
    if (md_data->playerReady[2]) {
        graphics_set_color(GREEN, BLACK);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 50, "PLAYER 3 (B)");
    }
    else {
        graphics_set_color(GRAY, BLACK);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 50, "PLAYER 3 (A)");
    }
    if (md_data->playerReady[3]) {
        graphics_set_color(GREEN, BLACK);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 35, "PLAYER 4 (B)");
    }
    else {
        graphics_set_color(GRAY, BLACK);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 35, "PLAYER 4 (A)");
    }

    bool any_ready = false;
    for (size_t i = 0; i < 4; ++i) {
        if (md_data->playerReady[i]) {
            any_ready = true;
            break;
        }
    }
    
    if (any_ready) {
        graphics_set_color(RED, BLACK);
        graphics_draw_text(disp, RES_X / 2 + 60, SCREEN_BOTTOM - 10, "PRESS START");
    }
    else {
        graphics_set_color(BLUE, BLACK);
        graphics_draw_text(disp, RES_X / 2 + 68, SCREEN_BOTTOM - 10, "A > READY");
    }

    graphics_set_color(GRAY, BLACK);
    graphics_draw_text(disp, SCREEN_LEFT + 20, SCREEN_BOTTOM - 10, "Press B to go back");
}

const char* get_minigame_name(int* offset_x) {
    *offset_x = 0;
    switch (selected_minigame) {
        case MINIGAME_FLYINGBATS:
            *offset_x = 50;
            return "Flying Bats";
        default:
            return "No MiniGame selected";
    }
}

const char* get_minigame_record() {
    switch (selected_minigame) {
        case MINIGAME_FLYINGBATS:
            {
                int time = game_save.times[MINIGAME_FLYINGBATS-1];
                size_t minutes = time / 60;
                size_t seconds = time % 60;
                snprintf(det_sc_time_string, 8, "%02d:%02d!", minutes, seconds);
                return det_sc_time_string;
            }
        default:
            return "No Record";
    }
}

const char* get_minigame_description() {
    switch (selected_minigame) {
        case MINIGAME_FLYINGBATS:
            return " Fly across the screen\n\nwhile dodging the wisps!";
        default:
            return "No description when\n\nthere's no minigame";
    }
}

const char* get_minigame_controls() {
    switch (selected_minigame) {
        case MINIGAME_FLYINGBATS:
            return "Stick\nD-Pad: Move\n\n\nA: Fly";
        default:
            return "No controls";
    }
}
