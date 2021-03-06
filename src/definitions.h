#pragma once

#include "minigames.h"

#define RES_X 320
#define RES_Y 240
#define SCREEN_RESOLUTION RESOLUTION_320x240
#define NUM_BUFFERS 2

#define SCREEN_BORDER 10
#define SCREEN_TOP SCREEN_BORDER
#define SCREEN_LEFT SCREEN_BORDER
#define SCREEN_RIGHT (RES_X - SCREEN_BORDER)
#define SCREEN_BOTTOM (RES_Y - SCREEN_BORDER)

#define DEADZONE 10

uint32_t TRANSP, WHITE, BLACK, RED, GREEN, BLUE, GRAY, DARK_GRAY;

#define RANDN(max) (rand() % (max))
#define RANDR(min, max) ((rand() % ((max) - (min) + 1)) + (min))

#define SECOND 1000000

typedef enum { SfxSet_None, SfxSet_Menu, SfxSet_Game } SfxSets;

uint8_t audio_current_sfx_set;
void audio_load_sfx_set(uint8_t set);
