#pragma once

#include "../definitions.h"
#include "../utils/mem_pool.h"

// game globals
extern MemZone memory_pool;
extern struct controller_data keys_held;
extern struct controller_data keys_released;
extern int connected_controllers;
extern SaveFile game_save;
extern MiniGame selected_minigame;
extern bool players_ready[4];
