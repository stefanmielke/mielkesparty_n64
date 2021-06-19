#pragma once

#include "../mielke_party.h"

#define DRAW_BACK_BUTTON()                                                                         \
	graphics_draw_sprite_trans_stride(disp, SCREEN_LEFT + 1, SCREEN_BOTTOM - 17, ui_sprites,       \
									  SPRITE_buttonBack)

typedef enum {
	SFX_BACK,
	SFX_CLICK,
	SFX_CONFIRM,
	SFX_CONTROLLER_CONNECTED,
	SFX_CONTROLLER_DISCONNECTED,
	SFX_UNCONFIRM,
	SFX_TOTAL_SOUNDS
} sfx_sounds_t;

typedef enum { BGM_NONE, BGM_INTRO, BGM_FLYING_BATS } bgm_sounds;
