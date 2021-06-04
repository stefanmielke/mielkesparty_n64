#include "minigame_flyingbats.h"

#include <stdio.h>

#include "../definitions.h"
#include "../utils/mem_pool.h"
#include "../utils/util_defs.h"
#include "minigame_defs.h"

#define GRAVITY 1
#define MAX_SPEED_UP 20
#define MAX_SPEED_DOWN 2
#define JUMP_SPEED 10
#define BUMP_SPEED 5
#define BORDER_SIZE (SCREEN_BORDER + 5)
#define LATERAL_SPEED 10.f / 255.f

#define MAX_ENEMIES 20
#define ENEMY_SIZE 10
#define ENEMY_SPEED_INIT 1.5f
#define ENEMY_SPEED_INC .1f

typedef struct fb_player_data {
    Rect rect;
    uint32_t color;
    Position speed;
    bool jumped;
    bool alive;
} FB_PlayerData;

typedef struct fb_enemy_data {
    Rect rect;
} FB_EnemyData;

typedef enum fb_state {
    FB_START,
    FB_PLAYING,
    FB_DYING,
    FB_DEAD
} FB_State;

typedef struct fb_minigame_data {
    FB_State state;
    size_t seconds;
    timer_link_t *timeTimer;
    timer_link_t *speedTimer;
    timer_link_t *postStartTimer;
    timer_link_t *postDeathTimer;
    FB_PlayerData playerOne;
    FB_EnemyData enemies[MAX_ENEMIES];
    float currentEnemySpeed;
} FB_MiniGameData;

FB_MiniGameData* fb_data;
char* fb_time_string; // format mm:ss\0

void increase_speed(int ovfl) {
    fb_data->currentEnemySpeed += ENEMY_SPEED_INC;
}

void post_start(int ovfl) {
    fb_data->state = FB_PLAYING;
}

void die() {
    stop_timer(fb_data->timeTimer);
    fb_data->playerOne.alive = false;
    fb_data->state = FB_DYING;
}

void post_death(int ovfl) {
    fb_data->state = FB_DEAD;
}

void time_inc(int ovfl) {
    ++fb_data->seconds;
    char minutes = fb_data->seconds / 60;
    char seconds = fb_data->seconds % 60;
    snprintf(fb_time_string, 8, "%02d:%02d", minutes, seconds);
}

void minigame_flyingbats_create() {
    fb_time_string = mem_zone_alloc(&memory_pool, sizeof(char)*8); // format mm:ss\0
    fb_time_string = "00:00\0";

    fb_data = mem_zone_alloc(&memory_pool, sizeof(FB_MiniGameData));
    fb_data->state = FB_START;
    fb_data->seconds = 0;

    fb_data->playerOne.rect.pos.x = 20;
    fb_data->playerOne.rect.pos.y = 20;
    fb_data->playerOne.rect.size.width = 20;
    fb_data->playerOne.rect.size.height = 20;
    fb_data->playerOne.color = BLUE;
    fb_data->playerOne.alive = true;

    for (size_t i = 0; i < MAX_ENEMIES; ++i) {
        fb_data->enemies[i].rect.size.width = ENEMY_SIZE;
        fb_data->enemies[i].rect.size.height = ENEMY_SIZE;
        fb_data->enemies[i].rect.pos.x = RANDN(RES_X) + (ENEMY_SIZE * 2) + RES_X;
        fb_data->enemies[i].rect.pos.y = RANDR(SCREEN_BORDER, SCREEN_BOTTOM);
    }
    fb_data->currentEnemySpeed = ENEMY_SPEED_INIT;

    fb_data->timeTimer = new_timer(TIMER_TICKS(SECOND), TF_CONTINUOUS, time_inc);
    fb_data->speedTimer = new_timer(TIMER_TICKS(5 * SECOND), TF_CONTINUOUS, increase_speed);
    fb_data->postStartTimer = new_timer(TIMER_TICKS(1 * SECOND), TF_ONE_SHOT, post_start);
}

void minigame_flyingbats_destroy() {
    delete_timer(fb_data->timeTimer);
    delete_timer(fb_data->speedTimer);
    delete_timer(fb_data->postStartTimer);
    delete_timer(fb_data->postDeathTimer);
}

bool minigame_flyingbats_tick() {
    if (fb_data->state == FB_DEAD || fb_data->state == FB_DYING)
        return true;

    /* player tick */
    fb_data->playerOne.speed.y += GRAVITY;
    if (fb_data->playerOne.speed.y < -MAX_SPEED_UP)
        fb_data->playerOne.speed.y = -MAX_SPEED_UP;
    if (fb_data->playerOne.speed.y > MAX_SPEED_DOWN)
        fb_data->playerOne.speed.y = MAX_SPEED_DOWN;

    fb_data->playerOne.rect.pos.y += fb_data->playerOne.speed.y;

    if (fb_data->playerOne.rect.pos.y - BORDER_SIZE < 0) {
        fb_data->playerOne.rect.pos.y = BORDER_SIZE;
        fb_data->playerOne.speed.y = 0;
    }
    else if (fb_data->playerOne.rect.pos.y + fb_data->playerOne.rect.size.height + BORDER_SIZE > RES_Y) {
        fb_data->playerOne.rect.pos.y = RES_Y - fb_data->playerOne.rect.size.height - BORDER_SIZE;
        fb_data->playerOne.speed.y = -BUMP_SPEED;
    }

    if (!fb_data->playerOne.jumped && keys_held.c[0].A) {
        fb_data->playerOne.jumped = true;
        fb_data->playerOne.speed.y = -JUMP_SPEED;
    }
    if (fb_data->playerOne.jumped && !keys_held.c[0].A) {
        fb_data->playerOne.jumped = false;
    }

    if (keys_held.c[0].x != 0)
        fb_data->playerOne.rect.pos.x += keys_held.c[0].x * LATERAL_SPEED;

    if (fb_data->playerOne.rect.pos.x - BORDER_SIZE < 0)
        fb_data->playerOne.rect.pos.x = BORDER_SIZE;
    else if (fb_data->playerOne.rect.pos.x + fb_data->playerOne.rect.size.width + BORDER_SIZE > RES_X)
        fb_data->playerOne.rect.pos.x = RES_X - fb_data->playerOne.rect.size.width - BORDER_SIZE;

    /* enemies tick */
    for (size_t i = 0; i < MAX_ENEMIES; ++i)
    {
        fb_data->enemies[i].rect.pos.x -= fb_data->currentEnemySpeed;
        if (fb_data->enemies[i].rect.pos.x < (ENEMY_SIZE * 2)) {
            fb_data->enemies[i].rect.pos.x = RANDN(RES_X) + 20 + RES_X;
            fb_data->enemies[i].rect.pos.y = RANDR(SCREEN_BORDER, SCREEN_BOTTOM);
        }
    }

    /* collision check */
    for (size_t i = 0; i < MAX_ENEMIES; ++i) {
        if (is_intersecting(fb_data->enemies[i].rect, fb_data->playerOne.rect)) {
            die();
            fb_data->postDeathTimer = new_timer(TIMER_TICKS(1 * SECOND), TF_ONE_SHOT, post_death);
            break;
        }
    }

    return true;
}

void minigame_flyingbats_display(display_context_t disp) {
    if (fb_data->state == FB_DEAD) {
        graphics_set_color(RED, BLACK);
        graphics_draw_text(disp, (RES_X/2)-20, (RES_Y/2)-20, "-DEAD-");
        graphics_set_color(WHITE, BLACK);
        graphics_draw_text(disp, (RES_X/2)-60, (RES_Y/2) + 10, "Total Time:");
        graphics_set_color(GREEN, BLACK);
        graphics_draw_text(disp, (RES_X/2)+30, (RES_Y/2) + 10, fb_time_string);
        return;
    }

    for (size_t i = 0; i < MAX_ENEMIES; ++i) {
        if (contains(fb_data->enemies[i].rect, screen_rect)) {
            DRAW_RECT(disp, fb_data->enemies[i].rect, RED);
        }
    }

    if (is_intersecting(fb_data->playerOne.rect, screen_rect)) {
        DRAW_RECT(disp, fb_data->playerOne.rect, fb_data->playerOne.alive ? fb_data->playerOne.color : GRAY);
    }

    if (fb_data->state == FB_START) {
        graphics_set_color(GREEN, BLACK);
        graphics_draw_text(disp, (RES_X/2)-30, (RES_Y/2)-20, "-START-");
    }
    else if (fb_data->state == FB_DYING) {
        graphics_set_color(RED, BLACK);
        graphics_draw_text(disp, (RES_X/2)-20, (RES_Y/2)-20, "-DEAD-");
    }

    graphics_set_color(WHITE, BLACK);
    graphics_draw_text(disp, SCREEN_LEFT, SCREEN_TOP, fb_time_string);
}
