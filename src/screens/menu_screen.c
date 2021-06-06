#include "menu_screen.h"

#include "screen_defs.h"

void menu_screen_repeat_timer_cntr_1(int ovfl) {
    menu_screen->canPress[0] = true;
    stop_timer(menu_screen->repeatTimer[0]);
}

void menu_screen_repeat_timer_cntr_2(int ovfl) {
    menu_screen->canPress[1] = true;
    stop_timer(menu_screen->repeatTimer[1]);
}

void menu_screen_repeat_timer_cntr_3(int ovfl) {
    menu_screen->canPress[2] = true;
    stop_timer(menu_screen->repeatTimer[2]);
}

void menu_screen_repeat_timer_cntr_4(int ovfl) {
    menu_screen->canPress[3] = true;
    stop_timer(menu_screen->repeatTimer[3]);
}

void menu_screen_end_tick() {
    for (int i = 0; i < 1; ++i) {
        if (i == 0 && !(connected_controllers & CONTROLLER_1_INSERTED))
            continue;
        else if (i == 1 && !(connected_controllers & CONTROLLER_2_INSERTED))
            continue;
        else if (i == 2 && !(connected_controllers & CONTROLLER_3_INSERTED))
            continue;
        else if (i == 3 && !(connected_controllers & CONTROLLER_4_INSERTED))
            continue;

        if (keys_held.c[i].up) {
            if (menu_screen->canPress[i]) {
                --menu_screen->currentMenuItem;
                menu_screen->canPress[i] = false;
                restart_timer(menu_screen->repeatTimer[i]);
            }
        }
        else if (keys_held.c[i].down) {
            if (menu_screen->canPress[i]) {
                ++menu_screen->currentMenuItem;
                menu_screen->canPress[i] = false;
                restart_timer(menu_screen->repeatTimer[i]);
            }
        }
        else {
            menu_screen->canPress[i] = true;
            stop_timer(menu_screen->repeatTimer[i]);
        }
    }

    if (menu_screen->currentMenuItem < 0)
        menu_screen->currentMenuItem = menu_screen->maxItem - 1;
    if (menu_screen->currentMenuItem >= menu_screen->maxItem)
        menu_screen->currentMenuItem = 0;
}

void menu_screen_create(size_t max_item) {
    menu_screen = mem_zone_alloc(&memory_pool, sizeof(MenuScreen));
    menu_screen->currentMenuItem = 0;
    menu_screen->maxItem = max_item;
    
    menu_screen->repeatTimer[0] = new_timer_stopped(TIMER_TICKS(SECOND / 5), TF_ONE_SHOT, menu_screen_repeat_timer_cntr_1);
    menu_screen->repeatTimer[1] = new_timer_stopped(TIMER_TICKS(SECOND / 5), TF_ONE_SHOT, menu_screen_repeat_timer_cntr_2);
    menu_screen->repeatTimer[2] = new_timer_stopped(TIMER_TICKS(SECOND / 5), TF_ONE_SHOT, menu_screen_repeat_timer_cntr_3);
    menu_screen->repeatTimer[3] = new_timer_stopped(TIMER_TICKS(SECOND / 5), TF_ONE_SHOT, menu_screen_repeat_timer_cntr_4);

    for (size_t i = 0; i < 4; ++i) {
        menu_screen->canPress[i] = true;
    }
}

void menu_screen_destroy() {
    delete_timer(menu_screen->repeatTimer[0]);
    delete_timer(menu_screen->repeatTimer[1]);
    delete_timer(menu_screen->repeatTimer[2]);
    delete_timer(menu_screen->repeatTimer[3]);
}
