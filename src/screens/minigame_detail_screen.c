#include "minigame_detail_screen.h"

#include <stdio.h>
#include <libdragon.h>

#include "../definitions.h"
#include "screen_defs.h"
#include "../minigames.h"
#include "../utils/mem_pool.h"

char* det_sc_time_string;
const char* get_minigame_name(int* offset_x);
const char* get_minigame_record();
const char* get_minigame_description();
const char* get_minigame_controls();

void minigame_detail_screen_create() {
    det_sc_time_string = mem_zone_alloc(&memory_pool, sizeof(char)*9); // format Record: mm:ss\0
    for (size_t i = 0; i < 4; ++i) {
        players_ready[i] = false;
    }
}

ScreenType minigame_detail_screen_tick() {
    for (int i = 0; i < 4; ++i) {
        if (players_ready[i]) {
            if (keys_released.c[i].start)
                return SCREEN_MINIGAME_PLAY;
            if (keys_released.c[i].B)
                players_ready[i] = false;
        }
        else {
            if (keys_released.c[i].A)
                players_ready[i] = true;
            if (keys_released.c[i].B)
                return SCREEN_INFINITE_MENU;
        }
    }

    return SCREEN_MINIGAME_DETAIL;
}

void minigame_detail_screen_display(display_context_t disp) {

    { // name
        int offset_x;
        graphics_set_color(BLUE, TRANSP);
        const char* name = get_minigame_name(&offset_x);
        graphics_draw_text(disp, SCREEN_LEFT + offset_x, SCREEN_TOP + 5, name);
    }
    { // record
        graphics_set_color(GREEN, TRANSP);
        graphics_draw_text(disp, RES_X / 2 - 4, SCREEN_TOP + 20, get_minigame_record());
    }
    // todo: draw minigame image/video
    { // description
        graphics_set_color(WHITE, TRANSP);
        graphics_draw_text(disp, SCREEN_LEFT, RES_Y / 2 + 20, get_minigame_description());
    }

    // separator
    graphics_draw_line(disp, RES_X / 2 + 46, SCREEN_TOP, RES_X / 2 + 46, SCREEN_BOTTOM, GRAY);

    // controls
    graphics_draw_text(disp, RES_X / 2 + 52, SCREEN_TOP + 30, get_minigame_controls());

    { // player controller info
        bool p1_connected = connected_controllers & CONTROLLER_1_INSERTED;
        uint32_t p1_color = players_ready[0] ? GREEN : p1_connected ? GRAY : DARK_GRAY;
        const char *p1_text = players_ready[0] ? "PLAYER 1 (B)" : "PLAYER 1 (A)";
        bool p2_connected = connected_controllers & CONTROLLER_2_INSERTED;
        uint32_t p2_color = players_ready[1] ? GREEN : p2_connected ? GRAY : DARK_GRAY;
        const char *p2_text = players_ready[0] ? "PLAYER 2 (B)" : "PLAYER 2 (A)";
        bool p3_connected = connected_controllers & CONTROLLER_3_INSERTED;
        uint32_t p3_color = players_ready[2] ? GREEN : p3_connected ? GRAY : DARK_GRAY;
        const char *p3_text = players_ready[0] ? "PLAYER 3 (B)" : "PLAYER 3 (A)";
        bool p4_connected = connected_controllers & CONTROLLER_4_INSERTED;
        uint32_t p4_color = players_ready[3] ? GREEN : p4_connected ? GRAY : DARK_GRAY;
        const char *p4_text = players_ready[0] ? "PLAYER 4 (B)" : "PLAYER 4 (A)";

        graphics_set_color(p1_color, TRANSP);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 80, p1_text);
        graphics_set_color(p2_color, TRANSP);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 65, p2_text);
        graphics_set_color(p3_color, TRANSP);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 50, p3_text);
        graphics_set_color(p4_color, TRANSP);
        graphics_draw_text(disp, RES_X / 2 + 56, SCREEN_BOTTOM - 35, p4_text);
    }

    { // ready text
        bool any_ready = false;
        for (size_t i = 0; i < 4; ++i) {
            if (players_ready[i]) {
                any_ready = true;
                break;
            }
        }

        if (any_ready) {
            graphics_set_color(RED, TRANSP);
            graphics_draw_text(disp, RES_X / 2 + 60, SCREEN_BOTTOM - 10, "PRESS START");
        }
        else {
            graphics_set_color(BLUE, TRANSP);
            graphics_draw_text(disp, RES_X / 2 + 68, SCREEN_BOTTOM - 10, "A > READY");
        }
    }

    { // go back
        graphics_set_color(GRAY, TRANSP);
        graphics_draw_text(disp, SCREEN_LEFT + 20, SCREEN_BOTTOM - 10, "Press B to go back");
    }
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
