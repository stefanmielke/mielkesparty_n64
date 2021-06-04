#pragma once

#include "../utils/mem_pool.h"

// game globals
extern MemZone memory_pool;
extern struct controller_data keys_held;
extern struct controller_data keys_released;
extern int connected_controllers;
