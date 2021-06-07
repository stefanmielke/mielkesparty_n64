/**
 * Original code by:
 * FlappyBird-N64 - audio.h
 * Copyright 2017, Christopher Bonhage
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <stdlib.h>
#include "../definitions.h"

/* Audio definitions */

#define AUDIO_FREQUENCY 32000
#define AUDIO_BUFFERS 4
#define MONO_PCM_SAMPLE_SIZE sizeof(s16)
#define STEREO_PCM_SAMPLE_SIZE (MONO_PCM_SAMPLE_SIZE << 1)

/* Sound FX definitions */

// Number of sfx it can play at a time. Last one is reserved for BGM.
#define SFX_NUM_CHANNELS AUDIO_BUFFERS

typedef enum sfx_sounds
{
    SFX_BACK,
    SFX_CLICK,
    SFX_CONFIRM,
    SFX_CONTROLLER_CONNECTED,
    SFX_CONTROLLER_DISCONNECTED,
    SFX_UNCONFIRM,
    SFX_TOTAL_SOUNDS
} sfx_sounds_t;

typedef enum
{
    BGM_NONE,
    BGM_INTRO,
    BGM_FLYING_BATS
} bgm_sounds;

typedef struct
{
    u16 sample_rate;
    u8 channels;
    u32 frames;
    u32 samples;
    s16 *data;
    bool loop;
} pcm_sound_t;

typedef struct
{
    u32 cursor;
    pcm_sound_t *sfx;
} sfx_channel_t;

typedef struct
{
    // Setup state
    u16 sample_rate;
    u32 frames;
    s16 *buffer;
    pcm_sound_t *sfx_cache[SFX_TOTAL_SOUNDS];

    bgm_sounds current_bgm;
    pcm_sound_t *bgm_sound;
    // Playback state
    sfx_channel_t channels[SFX_NUM_CHANNELS];
} audio_t;

/* Audio functions */

audio_t *audio_setup(const u16 sample_rate);
void audio_free(audio_t *audio);

void audio_tick(audio_t *audio);
void audio_play_sfx(audio_t *audio, const sfx_sounds_t sfx_sound);
void audio_load_and_play_bgm(audio_t *audio, bgm_sounds bgm_sound);

pcm_sound_t *read_dfs_pcm_sound(const char *file, u16 sample_rate, u8 channels, bool loop);

#define PLAY_AUDIO(audio) audio_play_sfx(audio_player, audio)
