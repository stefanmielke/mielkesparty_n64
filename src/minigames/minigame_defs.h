#pragma once

#include "../mielke_party.h"
#include "../screens/minigame_detail_screen.h"

#define DRAW_RECT(disp, rect, color)                                                               \
	(graphics_draw_box(disp, rect.pos.x, rect.pos.y, rect.size.width, rect.size.height, color))

typedef enum { BGM_NONE, BGM_INTRO, BGM_FLYING_BATS, BGM_JUMP_ROPE } bgm_sounds;
