#include "minigame_flyingbats.h"

#include "../definitions.h"
#include "../utils/mem_pool.h"
#include "../utils/util_defs.h"
#include "minigame_defs.h"

#define GRAVITY 1
#define MAX_SPEED_UP 20
#define MAX_SPEED_DOWN 2
#define JUMP_SPEED 10
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
    bool collided;
} FB_PlayerData;

typedef struct fb_enemy_data {
    Rect rect;
} FB_EnemyData;

typedef struct fb_minigame_data {
    timer_link_t *speed_timer;
    FB_PlayerData playerOne;
    FB_EnemyData enemies[MAX_ENEMIES];
    float currentEnemySpeed;
} FB_MiniGameData;

FB_MiniGameData* fb_data;

void increase_speed(int ovfl) {
    fb_data->currentEnemySpeed += ENEMY_SPEED_INC;
}

void minigame_flyingbats_create() {
    fb_data = mem_zone_alloc(&memory_pool, sizeof(FB_MiniGameData));
    fb_data->playerOne.rect.pos.x = 20;
    fb_data->playerOne.rect.pos.y = 20;
    fb_data->playerOne.rect.size.width = 20;
    fb_data->playerOne.rect.size.height = 20;
    fb_data->playerOne.color = BLUE;

    for (size_t i = 0; i < MAX_ENEMIES; ++i) {
        fb_data->enemies[i].rect.size.width = ENEMY_SIZE;
        fb_data->enemies[i].rect.size.height = ENEMY_SIZE;
        fb_data->enemies[i].rect.pos.x = RANDN(RES_X) + (ENEMY_SIZE * 2) + RES_X;
        fb_data->enemies[i].rect.pos.y = RANDR(SCREEN_BORDER, SCREEN_BOTTOM);
    }
    fb_data->currentEnemySpeed = ENEMY_SPEED_INIT;

    fb_data->speed_timer = new_timer(TIMER_TICKS(5 * SECOND), TF_CONTINUOUS, increase_speed);
}

void minigame_flyingbats_destroy() {
    delete_timer(fb_data->speed_timer);
}

bool minigame_flyingbats_tick() {
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
    else if (fb_data->playerOne.rect.pos.y + fb_data->playerOne.rect.size.height + BORDER_SIZE > RES_Y)
        fb_data->playerOne.rect.pos.y = RES_Y - fb_data->playerOne.rect.size.height - BORDER_SIZE;

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
    fb_data->playerOne.collided = false;
    for (size_t i = 0; i < MAX_ENEMIES; ++i)
    {
        if (is_intersecting(fb_data->enemies[i].rect, fb_data->playerOne.rect)) {
            fb_data->playerOne.collided = true;
            break;
        }
    }

    return true;
}

void minigame_flyingbats_display(display_context_t disp) {
    for (size_t i = 0; i < MAX_ENEMIES; ++i)
    {
        if (contains(fb_data->enemies[i].rect, screen_rect)) {
            DRAW_RECT(disp, fb_data->enemies[i].rect, RED);
        }
    }

    if (is_intersecting(fb_data->playerOne.rect, screen_rect)) {
        DRAW_RECT(disp, fb_data->playerOne.rect, fb_data->playerOne.collided ? GREEN : fb_data->playerOne.color);
    }
}
