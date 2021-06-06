#pragma once

#include <stddef.h>

#include <libdragon.h>

typedef struct menu_screen {
    char currentMenuItem;
    char maxItem;
    timer_link_t* repeatTimer[4];
    bool canPress[4];
} MenuScreen;

MenuScreen* menu_screen;

void menu_screen_create(size_t max_item);
void menu_screen_destroy();

void menu_screen_end_tick();
