#pragma once

#include <stddef.h>

#include "../definitions.h"

typedef struct {
	// time for each minigame (disregarding MINIGAME_NONE, so use -1 when using it)
	int times[MINIGAME_MAX - 1];
} SaveFileBody;

typedef struct {
	uint8_t check;
	SaveFileBody body;
} SaveFile;

void save_write(SaveFile save);
SaveFile save_read();