# Mielke's Party

An N64 game that contains minigames to play alone or with your friends.

---
## Building

This game uses [libdragon](https://github.com/DragonMinded/libdragon) as a library.

To build, you can either install the library and run ```make all``` or you can install Docker and run ```build-prepare.sh```, ```build-libs.sh``` and ```build.sh```.

---
## Folders and Files

### mielke_party.c

Main file of the program. Has the `main` function and load and change between `screens`.

### Screens

Screens are the current state of the game (where the player currently is). It can be a menu, or it can be the `minigame_play_screen` (that contains the "play" state).

Also has `screen_config.h` and `screen_defs.h`, that are used by the screens to load a few global vars and defines.

Has the following main methods (defined on `screen_config.h`):

> void (*fnCreate)()

eg.: `void main_screen_create()`

Called once when changing to that screen.

> ScreenType (*fnTick)()

eg.: `ScreenType main_screen_tick()`

Called once per frame to update the state of the game. It doesn't render anything.

It should return the next screen if it changed (eg.: player selected an item on the menu), or the current `ScreenType` if it shouldn't change.

> void (*fnDisplay)(display_context_t disp)

eg.: `void main_screen_display(display_context_t disp)`

Called once per frame to draw the current state to the screen. It shouldn't update the state.

### Minigames

Every minigame is called by `minigame_play_screen.c` and has some configurations to be done on `minigame_detail_screen.c`.

**minigame_play_screen.c**

On this file, there are a few method that have to be updated when adding a new minigame. Those are the below:

> const char* get_minigame_name(int* offset_x)

Returns the name of the minigame

> const char* get_minigame_record()

Get a formatted string with the current record for that minigame (eg.: '12:34')

You can find the current record for a minigame using `game_save.times[MINIGAME_*-1]`.

> const char* get_minigame_description()

Returns the description for that minigame. Keep in mind that there's not much width space and test that on the screen.

> void open_minigame_thumb()

Sets the `md_thumb` variable with the sprite for the thumbnail. When adding a minigame add the sprite path in the `switch`.

> void draw_minigame_controls(display_context_t disp, int start_x, int start_y)

Draw the minigame controls on the screen. `start_x` and `start_y` are the coords on the screen where it should start drawing.

**minigame_play_screen.c**

You have to update the following method when creating a new minigame:

> void minigame_play_screen_create()

Set the function pointers to the minigame functions.

**Minigame Implementation**

Every minigame file has the same methods that it has to implement to funcion properly. Those are (as defined on `minigames.h`):

> void (*fnGameCreate)()

eg.: `void minigame_flyingbats_create();`

Is called once by `minigame_play_screen_create()` after it initializes itself.

This function should load any dynamic memory and sprites.

> bool (*fnGameTick)()

eg.: `bool minigame_flyingbats_tick();`

Is called every frame by `minigame_play_screen_tick()`, and is responsible for updating the state of the game, and reacting to inputs.

Should return `true` if the game is not over yet, and `false` if the game should return to the `minigame_detail_screen`.

> void (*fnGameDisplay)(display_context_t disp)

eg.: `void minigame_flyingbats_display(display_context_t disp);`

Called every frame after `fnGameTick` by `minigame_play_screen_display(display_context_t disp)`.

It should draw the current state to the screen.

> void (*fnGameDestroy)()

eg.: `void minigame_flyingbats_destroy();`

Is called once `fnGameTick` returns `false` or an external action causes the game to be finished early (eg.: player exits the game).

### FAQ

> How do I load a sprite?

When creating sprites you should either use [Texture Packer]() with [this plugin]() or create the files created by the exporter manually.

When exporting using the plugin, you should put the spritesheet at `src/raw` and the header files at `src/gfx_h`.

After that you can use the macro `alloc_and_load_spritesheet_NAME_OF_SPRITE` to allocate and load your sprite. For example:

```c
alloc_and_load_spritesheet_NAME_OF_SPRITE(sprite_ptr);
```

> How do I draw a sprite?

You can use software rendering (easier method) like below:

```c
// draws a sprite:
graphics_draw_sprite(disp, x_pos, y_pos, sprite_ptr);
// draw a part of the sprite (if using a spritesheet):
graphics_draw_sprite_trans_stride(disp, x_pos, y_pos, sprite_ptr, spritesheet_offset);

// more examples on 'minigame_detail_screen.c'
```

Or you can use hardware rendering (a bit more difficult, but more powerful) like this:

```c
// begin rendering (beginning of the function)
rdp_sync(SYNC_PIPE);
rdp_set_default_clipping();
rdp_enable_texture_copy();
rdp_attach_display(disp);

// draw a sprite:
rdp_sync(SYNC_PIPE);
rdp_load_texture_stride(0 /*text_slot should be zero*/, 0 /*text_loc should be zero*/, MIRROR_DISABLED, sprite_ptr, spritesheet_offset);
rdp_draw_sprite(0, x_pos, y_pos, MIRROR_DISABLED);

// end rendering (end of the function)
rdp_detach_display();

// more examples on 'minigame_flyingbats.c'
```

You can mix and match both styles at any time (just remember to attach and detach before and after using hardware rendering).

> How do I allocate memory?

For sprites and sfx you should use `malloc`. There are issues preventing the use of the current memory allocator for those use-cases.

For any non-sprite and non-sfx you should use the following code instead of malloc:

```c
my_var = mem_zone_alloc(&memory_pool, sizeof(MyStruct));
```

This code will ensure you don't need to call `free` and is automatically collected when changing screens.

**Caution**: if you don't initialize your variables, a previous state can be used and your variables may not behave how you want. So remember to initialize any variable you create with this function with a default value.

> How do I play background music?

You should use the method `audio_load_and_play_bgm` like below:

audio_load_and_play_bgm(audio_player, BGM_YOUR_BGM);

> How do I play sounds?

For any sounds, you should use the macro `PLAY_AUDIO(audio)` defined on `audio.h` passing the correct audio enum to play.

### Utils

There are some utils that were created to aid the development. Some of those were copied from other projects and have their copyright on the beginning of the file.

**audio.c\h**

Has audio loading and playback.

**mem_pool.c\h**

Has the memory pool implementation used to allocate dynamic memory.

**util_defs.c\h**

Has `Position`, `Rect`, and `Size` structs along with some functions to ease their use.
