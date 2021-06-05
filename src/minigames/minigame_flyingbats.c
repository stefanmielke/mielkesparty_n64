#include "minigame_flyingbats.h"

#include <stdio.h>
#include <string.h>

#include "../definitions.h"
#include "../utils/mem_pool.h"
#include "../utils/util_defs.h"
#include "minigame_defs.h"
#include "../gfx_h/gfx_flying_bats.h"

#define GRAVITY 1
#define MAX_SPEED_UP 20
#define MAX_SPEED_DOWN 2
#define JUMP_SPEED 10
#define BUMP_SPEED 5
#define BORDER_SIZE (SCREEN_BORDER + 5)
#define LATERAL_SPEED 10.f / 255.f

#define MAX_ENEMIES 20
#define ENEMY_SIZE 14
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
    bool wantsToSave;
    bool saving;
    bool isNewRecord;
    size_t secondsRecord;
    size_t seconds;
    size_t animCounter;
    timer_link_t *animTimer;
    timer_link_t *timeTimer;
    timer_link_t *speedTimer;
    timer_link_t *postStartTimer;
    timer_link_t *postDeathTimer;
    FB_PlayerData playerOne;
    FB_EnemyData enemies[MAX_ENEMIES];
    float currentEnemySpeed;
    sprite_t *sprites;
} FB_MiniGameData;

FB_MiniGameData* fb_data;
char* fb_time_string; // format mm:ss\0
char* fb_record_string; // format mm:ss\0

void increase_speed(int ovfl) {
    fb_data->currentEnemySpeed += ENEMY_SPEED_INC;
}

void post_start(int ovfl) {
    fb_data->state = FB_PLAYING;
}

void die() {
    stop_timer(fb_data->animTimer);
    stop_timer(fb_data->timeTimer);

    // check if record
    if (fb_data->seconds > fb_data->secondsRecord) {
        fb_data->isNewRecord = true;
        game_save.times[MINIGAME_FLYINGBATS-1] = fb_data->seconds;
    }

    fb_data->playerOne.alive = false;
    fb_data->state = FB_DYING;
}

void post_death(int ovfl) {
    fb_data->state = FB_DEAD;
    fb_data->wantsToSave = true;
}

void set_time() {
    char minutes = fb_data->seconds / 60;
    char seconds = fb_data->seconds % 60;
    snprintf(fb_time_string, 8, "%02d:%02d", minutes, seconds);
}

void time_inc(int ovfl) {
    ++fb_data->seconds;
    set_time();
}

void anim_update(int ovfl) {
    ++fb_data->animCounter;
}

void minigame_flyingbats_create() {
    fb_data = mem_zone_alloc(&memory_pool, sizeof(FB_MiniGameData));
    fb_data->state = FB_START;
    fb_data->seconds = 0;
    fb_data->saving = false;
    fb_data->wantsToSave = false;
    fb_data->isNewRecord = false;
    fb_data->secondsRecord = game_save.times[MINIGAME_FLYINGBATS-1];

    fb_time_string = mem_zone_alloc(&memory_pool, sizeof(char)*9); // format mm:ss\0
    set_time();

    fb_record_string = mem_zone_alloc(&memory_pool, sizeof(char)*9); // format - mm:ss\0
    char record_minutes = fb_data->secondsRecord / 60;
    char record_seconds = fb_data->secondsRecord % 60;
    snprintf(fb_record_string, 8, "%02d:%02d", record_minutes, record_seconds);

    fb_data->playerOne.rect.pos.x = 20;
    fb_data->playerOne.rect.pos.y = 20;
    fb_data->playerOne.rect.size.width = 11;
    fb_data->playerOne.rect.size.height = 11;
    fb_data->playerOne.color = BLUE;
    fb_data->playerOne.alive = true;

    for (size_t i = 0; i < MAX_ENEMIES; ++i) {
        fb_data->enemies[i].rect.size.width = ENEMY_SIZE;
        fb_data->enemies[i].rect.size.height = ENEMY_SIZE;
        fb_data->enemies[i].rect.pos.x = RANDN(RES_X) + (ENEMY_SIZE * 2) + RES_X;
        fb_data->enemies[i].rect.pos.y = RANDR(SCREEN_BORDER, SCREEN_BOTTOM);
    }
    fb_data->currentEnemySpeed = ENEMY_SPEED_INIT;

    fb_data->animTimer = new_timer(TIMER_TICKS(SECOND / 10), TF_CONTINUOUS, anim_update);
    fb_data->timeTimer = new_timer(TIMER_TICKS(SECOND), TF_CONTINUOUS, time_inc);
    fb_data->speedTimer = new_timer(TIMER_TICKS(5 * SECOND), TF_CONTINUOUS, increase_speed);
    fb_data->postStartTimer = new_timer(TIMER_TICKS(1 * SECOND), TF_ONE_SHOT, post_start);

    alloc_and_load_spritesheet_flying_bat(fb_data->sprites);
}

void minigame_flyingbats_destroy() {
    free(fb_data->sprites);
    delete_timer(fb_data->animTimer);
    delete_timer(fb_data->timeTimer);
    delete_timer(fb_data->speedTimer);
    delete_timer(fb_data->postStartTimer);
    delete_timer(fb_data->postDeathTimer);
}

bool minigame_flyingbats_tick() {
    if (fb_data->saving) {
        save_write(game_save);

        fb_data->saving = false;
        fb_data->wantsToSave = false;
    }
    if (fb_data->wantsToSave) {
        fb_data->saving = true;
    }

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
        graphics_set_color(fb_data->isNewRecord ? GREEN : WHITE, BLACK);
        graphics_draw_text(disp, (RES_X/2)+30, (RES_Y/2) + 10, fb_time_string);

        if (fb_data->wantsToSave) {
            graphics_set_color(RED, BLACK);
            graphics_draw_text(disp, (RES_X/2)-30, SCREEN_BOTTOM - 20, "SAVING NEW TIME...");
        }
        return;
    }

    /* Assure RDP is ready for new commands */
    rdp_sync( SYNC_PIPE );
    rdp_set_default_clipping();
    rdp_enable_texture_copy();
    rdp_attach_display(disp);

    rdp_sync(SYNC_PIPE);
    rdp_load_texture_stride(0, 0, MIRROR_DISABLED, fb_data->sprites, SPRITE_fb_sky);
    rdp_draw_textured_rectangle(0, 0, 0, RES_X, RES_Y, MIRROR_DISABLED);

    for (size_t i = 0; i < MAX_ENEMIES; ++i) {
        if (contains(fb_data->enemies[i].rect, screen_rect)) {
            rdp_sync(SYNC_PIPE);
            rdp_load_texture_stride(0, 0, MIRROR_DISABLED, fb_data->sprites, (fb_data->animCounter % 2) + SPRITE_fb_obstacle_1);
            rdp_draw_sprite(0, fb_data->enemies[i].rect.pos.x - 9, fb_data->enemies[i].rect.pos.y - 9, MIRROR_DISABLED);
        }
    }

    if (is_intersecting(fb_data->playerOne.rect, screen_rect)) {
        size_t animCounter = fb_data->playerOne.speed.y >= 0 ? fb_data->animCounter : fb_data->animCounter*2;
        rdp_sync(SYNC_PIPE);
        rdp_load_texture_stride(0, 0, MIRROR_DISABLED, fb_data->sprites, animCounter % (SPRITE_fb_bat_4 - SPRITE_fb_bat_1 + 1));
        rdp_draw_sprite(0, fb_data->playerOne.rect.pos.x - 11, fb_data->playerOne.rect.pos.y - 13, MIRROR_DISABLED);
    }

    rdp_detach_display();

    if (fb_data->state == FB_START) {
        graphics_set_color(GREEN, BLACK);
        graphics_draw_text(disp, (RES_X/2)-30, (RES_Y/2)-20, "-START-");
    }
    else if (fb_data->state == FB_DYING) {
        graphics_set_color(RED, BLACK);
        graphics_draw_text(disp, (RES_X/2)-20, (RES_Y/2)-20, "-DEAD-");
    }

    graphics_set_color(WHITE, TRANSP);
    graphics_draw_text(disp, SCREEN_LEFT, SCREEN_TOP, fb_time_string);
    graphics_set_color(GRAY, TRANSP);
    graphics_draw_text(disp, SCREEN_LEFT + 45, SCREEN_TOP, fb_record_string);
}
