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

// Shutdown music
static void I_OGB_ShutdownMusic(void){
	// TODO(gmb)
}

// Initialize music subsystem
static bool I_OGB_InitMusic(void){
	// TODO(gmb)
	return true;
}

// Set music volume (0 - 127)
static void I_OGB_SetMusicVolume(int volume){
	// TODO(gmb)
}

// Start playing a mid
static void I_OGB_PlaySong(void *handle, bool looping){
	// TODO(gmb)
}

static void I_OGB_PauseSong(void){
	// TODO(gmb)
}

static void I_OGB_ResumeSong(void){
	// TODO(gmb)
}

static void I_OGB_StopSong(void){
	// TODO(gmb)
}

static void I_OGB_UnRegisterSong(void *handle){
	// TODO(gmb)
}

static void *I_OGB_RegisterSong(void *data, int len){
	// TODO(gmb)
	return NULL;
}

// Is the song playing?
static bool I_OGB_MusicIsPlaying(void){
	// TODO(gmb)
	return false;
}

// Poll music position; if we have passed the loop point end position
// then we need to go back.
static void I_OGB_PollMusic(void){
	// TODO(gmb)
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

