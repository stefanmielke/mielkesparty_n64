#include "minigame_flyingbats.h"

#include "../definitions.h"
#include "../utils/mem_pool.h"
#include "minigame_defs.h"

#define GRAVITY 3
#define MAX_SPEED_UP 20
#define MAX_SPEED_DOWN 3
#define JUMP_SPEED 14
#define BORDER_SIZE 5
#define LATERAL_SPEED 10.f / 255.f

typedef struct minigame_data {
    Position playerOnePosition;
    Size playerOneSize;
    uint32_t playerOneColor;
    Position playerOneSpeed;
    bool playerOneJumped;
} MiniGameData;

MiniGameData* fb_data;

void minigame_flyingbats_create() {
    fb_data = mem_zone_alloc(&memory_pool, sizeof(MiniGameData));
    fb_data->playerOnePosition.x = 20;
    fb_data->playerOnePosition.y = 20;
    fb_data->playerOneSize.width = 20;
    fb_data->playerOneSize.height = 20;
    fb_data->playerOneColor = BLUE;
}

bool minigame_flyingbats_tick() {
    fb_data->playerOneSpeed.y += GRAVITY;
    if (fb_data->playerOneSpeed.y < -MAX_SPEED_UP)
        fb_data->playerOneSpeed.y = -MAX_SPEED_UP;
    if (fb_data->playerOneSpeed.y > MAX_SPEED_DOWN)
        fb_data->playerOneSpeed.y = MAX_SPEED_DOWN;

    fb_data->playerOnePosition.y += fb_data->playerOneSpeed.y;

    if (fb_data->playerOnePosition.y - BORDER_SIZE < 0)
        fb_data->playerOnePosition.y = BORDER_SIZE;
    else if (fb_data->playerOnePosition.y + fb_data->playerOneSize.height + BORDER_SIZE > RES_Y)
        fb_data->playerOnePosition.y = RES_Y - fb_data->playerOneSize.height - BORDER_SIZE;

    if (fb_data->playerOnePosition.x - BORDER_SIZE < 0)
        fb_data->playerOnePosition.x = BORDER_SIZE;
    else if (fb_data->playerOnePosition.x + fb_data->playerOneSize.width + BORDER_SIZE > RES_X)
        fb_data->playerOnePosition.x = RES_X - fb_data->playerOneSize.width - BORDER_SIZE;

    if (!fb_data->playerOneJumped && keys_held.c[0].A) {
        fb_data->playerOneJumped = true;
        fb_data->playerOneSpeed.y = -JUMP_SPEED;
    }
    if (keys_released.c[0].A) {
        fb_data->playerOneJumped = false;
    }

    if (keys_held.c[0].x != 0)
        fb_data->playerOnePosition.x += keys_held.c[0].x * LATERAL_SPEED;

    return true;
}

void minigame_flyingbats_display(display_context_t disp) {
    if (fb_data->playerOnePosition.y > 0 && fb_data->playerOnePosition.y < RES_Y
        && fb_data->playerOnePosition.x > 0 && fb_data->playerOnePosition.x < RES_X) {
        graphics_draw_box(disp, fb_data->playerOnePosition.x, fb_data->playerOnePosition.y, fb_data->playerOneSize.width, fb_data->playerOneSize.height, fb_data->playerOneColor);
    }
}
