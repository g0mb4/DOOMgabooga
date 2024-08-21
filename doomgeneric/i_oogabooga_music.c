//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	System interface for music.
//

#include "doomtype.h"
#include "memio.h"
#include "mus2mid.h"

#include "deh_str.h"
#include "gusconf.h"
#include "i_sound.h"
#include "i_system.h"
#include "i_swap.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "sha1.h"
#include "w_wad.h"
#include "z_zone.h"

#define MAXMIDLENGTH (96 * 1024)
#define MID_HEADER_MAGIC "MThd"
#define MUS_HEADER_MAGIC "MUS\x1a"

static Audio_Player *music_player = NULL;
static Audio_Source *current_track = NULL;

// Shutdown music
static void I_OGB_ShutdownMusic(void){
	// NOTE(gmb): OS will handle this
}

// Initialize music subsystem
static bool I_OGB_InitMusic(void){
	music_player = audio_player_get_one();
	assert(music_player, "Unable to get an audio player for music");

	return true;
}

// Set music volume (0 - 127)
static void I_OGB_SetMusicVolume(int volume){
	music_player->config.volume = set_volume(volume);
}

// Start playing a mid
static void I_OGB_PlaySong(void *handle, bool looping){
	if (!handle){
		return;
	}

	if (current_track){
		audio_source_destroy((Audio_Source *)current_track);
		current_track = NULL;
	}
	audio_player_clear_source(music_player);

	current_track = (Audio_Source *)handle;

	audio_player_set_source(music_player, *current_track);
	audio_player_set_looping(music_player, looping);
	audio_player_set_state(music_player, AUDIO_PLAYER_STATE_PLAYING);
}

static void I_OGB_PauseSong(void){
 	audio_player_set_state(music_player, AUDIO_PLAYER_STATE_PAUSED);
}

static void I_OGB_ResumeSong(void){
	audio_player_set_state(music_player, AUDIO_PLAYER_STATE_PLAYING);
}

static void I_OGB_StopSong(void){
	if (current_track){
		audio_source_destroy((Audio_Source *)current_track);
		current_track = NULL;
	}
	audio_player_clear_source(music_player);

	audio_player_set_state(music_player, AUDIO_PLAYER_STATE_PAUSED);
}

static void I_OGB_UnRegisterSong(void *handle){
	if (current_track){
		audio_source_destroy((Audio_Source *)current_track);
		current_track = NULL;
	}
	audio_player_clear_source(music_player);
}

// Determine whether memory block is a .mid file
static bool IsMid(u8 *mem, int len){
    return len > 4 && !memcmp(mem, MID_HEADER_MAGIC, 4);
}

static bool ConvertMus(byte *musdata, int len, char *filename){
    MEMFILE *instream;
	MEMFILE *outstream;
    void *outbuf;
    size_t outbuf_len;
    int result;

    instream = mem_fopen_read(musdata, len);
    outstream = mem_fopen_write();

    result = mus2mid(instream, outstream);
	assert(result == 0, "mus2mid failed");

    if (result == 0) {
        mem_get_buf(outstream, &outbuf, &outbuf_len);
        result = M_WriteFile(filename, outbuf, outbuf_len);
    }

    mem_fclose(instream);
    mem_fclose(outstream);

    return result;
}

static Audio_Source* load_midi(const char * path) {
	// TODO: load MIDI here
	printf("Loading MIDI file: %cs\n", path);
	string data;

	bool ok = os_read_entire_file_f(path, &data, get_heap_allocator());
	assert(ok, "Unable to read file: %cs", path);

	printf("Loaded: %u bytes\n", data.count);

	return NULL;
}

static void *I_OGB_RegisterSong(void *data, int len){
	// TODO(gmb): Do it in memory
	char * temp_name = M_TempFile("doom.mid");
	if (IsMid(data, len) && len < MAXMIDLENGTH){
		// MIDI
		const bool ok = M_WriteFile(temp_name, data, len);
		assert(ok, "Unable to write: %cs", temp_name);
	} else {
		// MUS
		const bool ok = ConvertMus(data, len, temp_name);
		assert(ok, "Unable to write: %cs", temp_name);
	}

	Audio_Source * music = load_midi(temp_name);
	dealloc(get_heap_allocator(), temp_name);
	return music;
}

// Is the song playing?
static bool I_OGB_MusicIsPlaying(void){
	// TODO(gmb): Do better.
	return current_track != NULL;
}

// Poll music position; if we have passed the loop point end position
// then we need to go back.
static void I_OGB_PollMusic(void){
	// NOTE(gmb): Nothing to do here.
}

static snddevice_t music_OGB_devices[] =
{
	SNDDEVICE_PAS,
	SNDDEVICE_GUS,
	SNDDEVICE_WAVEBLASTER,
	SNDDEVICE_SOUNDCANVAS,
	SNDDEVICE_GENMIDI,
	SNDDEVICE_AWE32,
};

music_module_t DG_music_module =
{
	music_OGB_devices,
	arrlen(music_OGB_devices),
	I_OGB_InitMusic,
	I_OGB_ShutdownMusic,
	I_OGB_SetMusicVolume,
	I_OGB_PauseSong,
	I_OGB_ResumeSong,
	I_OGB_RegisterSong,
	I_OGB_UnRegisterSong,
	I_OGB_PlaySong,
	I_OGB_StopSong,
	I_OGB_MusicIsPlaying,
	I_OGB_PollMusic,
};

