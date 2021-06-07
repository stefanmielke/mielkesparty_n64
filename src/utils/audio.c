/**
 * Original code by:
 * FlappyBird-N64 - audio.c
 * Copyright 2017, Christopher Bonhage
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "audio.h"

audio_t *audio_setup(const u16 sample_rate)
{
    const u16 buffer_length = audio_get_buffer_length();
    s16 *buffer = malloc(buffer_length * STEREO_PCM_SAMPLE_SIZE);
    audio_t *audio = malloc(sizeof(audio_t));
    audio->sample_rate = sample_rate;
    audio->frames = buffer_length << 1;
    audio->buffer = buffer;

    /* Load the sound effects cache */
    const char *sfx_files[SFX_TOTAL_SOUNDS] = {
        "/sfx/back.raw",
        "/sfx/click.raw",
        "/sfx/confirm.raw",
        "/sfx/controller_connected.raw",
        "/sfx/controller_unconnected.raw",
        "/sfx/unconfirm.raw"
    };
    pcm_sound_t *sfx;
    for (u8 i = 0; i < SFX_TOTAL_SOUNDS; i++) {
        sfx = read_dfs_pcm_sound(sfx_files[i], sample_rate, 1, false);
        audio->sfx_cache[i] = sfx;
    }
    /* Setup the sound effects channels */
    for (u8 i = 0; i < SFX_NUM_CHANNELS; i++) {
        audio->channels[i].cursor = 0;
        audio->channels[i].sfx = NULL;
    }
    return audio;
}

void audio_load_and_play_bgm(audio_t *audio, bgm_sounds bgm_sound) {
    if (bgm_sound == audio->current_bgm || bgm_sound == BGM_NONE)
        return;

    if (audio->current_bgm != BGM_NONE) {
        free(audio->channels[SFX_NUM_CHANNELS - 1].sfx->data);
        free(audio->channels[SFX_NUM_CHANNELS - 1].sfx);
    }

    const char *bgm_path;
    switch (bgm_sound)
    {
    case BGM_INTRO:
        bgm_path = "/sfx/intro.raw";
        break;
    case BGM_FLYING_BATS:
        bgm_path = "/sfx/fb_music.raw";
        break;
    default:
        bgm_path = NULL;
        break;
    }

    if (!bgm_path)
        return;

    pcm_sound_t *sfx = read_dfs_pcm_sound(bgm_path, audio->sample_rate, 2, true);
    audio->current_bgm = bgm_sound;
    audio->channels[SFX_NUM_CHANNELS - 1].sfx = sfx;
    audio->channels[SFX_NUM_CHANNELS - 1].cursor = 0;
}

void audio_free(audio_t *audio) {
    /* Clear the sound effects cache */
    for (u8 i = 0; i < SFX_TOTAL_SOUNDS; i++) {
        free(audio->sfx_cache[i]->data);
        free(audio->sfx_cache[i]);
        audio->sfx_cache[i] = NULL;
    }
    /* Clear sound effects pointers from playback channels */
    for (u8 i = 0; i < SFX_NUM_CHANNELS; i++) {
        audio->channels[i].sfx = NULL;
    }
    /* Shut down the audio subsystem */
    free(audio->buffer);
    free(audio);
}

inline static s16 mix_pcm_samples(s32 mix, u8 num)
{
    return (num > 1) ? mix / num : mix;
}

void audio_tick(audio_t *audio)
{
    if (audio && audio_can_write())
    {
        sfx_channel_t channel;
        pcm_sound_t *sfx;
        s32 left_mix, right_mix;
        u8 left_num, right_num;
        /* Fill the audio buffer with stereo sample frames */
        for (u16 frame = 0; frame < audio->frames;
             left_mix = left_num = right_mix = right_num = 0) {
            /* Accumulate all currently playing sound effects samples */
            for (u8 i = 0; i < SFX_NUM_CHANNELS; i++) {
                channel = audio->channels[i];
                sfx = channel.sfx;
                if (sfx && channel.cursor < sfx->samples) {
                    left_mix += sfx->data[channel.cursor++];
                    left_num++;
                    /* Play mono sound effects in both speakers */
                    if (sfx->channels == 1) {
                        right_mix += sfx->data[channel.cursor - 1];
                        right_num++;
                    }
                    /* Play stereo sound effects in separate speakers */
                    else if (channel.cursor < sfx->samples) {
                        right_mix += sfx->data[channel.cursor++];
                        right_num++;
                    }

                    /* Reset channels that have finished playing */
                    if (channel.cursor >= sfx->samples) {
                        channel.cursor = 0;
                        if (!sfx->loop)
                            channel.sfx = NULL;
                    }
                }
                audio->channels[i] = channel;
            }
            /* Mix down all of the samples as an average */
            audio->buffer[frame++] = mix_pcm_samples(left_mix, left_num);
            audio->buffer[frame++] = mix_pcm_samples(right_mix, right_num);
        }
        audio_write(audio->buffer);
    }
}

void audio_play_sfx(audio_t *audio, const sfx_sounds_t sfx_sound)
{
    if (audio) {
        pcm_sound_t *sfx = audio->sfx_cache[sfx_sound];
        if (sfx) {
            for (u8 i = 0; i < SFX_NUM_CHANNELS - 1; i++) {
                if (!audio->channels[i].sfx) {
                    audio->channels[i].sfx = sfx;
                    audio->channels[i].cursor = 0;
                    break;
                }
            }
        }
    }
}

pcm_sound_t *read_dfs_pcm_sound(const char *file, u16 sample_rate, u8 channels, bool loop)
{
    int fp = dfs_open(file);
    const u32 size = dfs_size(fp);
    s16 *data = malloc(size);
    dfs_read(data, 1, size, fp);
    dfs_close(fp);
    u32 frames = size / MONO_PCM_SAMPLE_SIZE;
    pcm_sound_t *sound = malloc(sizeof(pcm_sound_t));
    sound->sample_rate = sample_rate;
    sound->channels = channels;
    sound->frames = frames;
    sound->samples = frames / channels;
    sound->data = data;
    sound->loop = loop;
    return sound;
}
