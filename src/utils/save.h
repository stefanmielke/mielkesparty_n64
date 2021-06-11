#pragma once

#include <stddef.h>

#include "../definitions.h"

typedef struct {
	// 0 if can't save, 1 if it can and is loaded
	uint8_t check;
	// time for each minigame (disregarding MINIGAME_NONE, so use -1 when using it)
	int times[MINIGAME_MAX - 1];
} SaveFile;

void save_write(SaveFile save);
SaveFile save_read();