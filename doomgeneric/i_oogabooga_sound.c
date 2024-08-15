//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2008 David Flater
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
//	System interface for sound.
//

#include "deh_str.h"
#include "i_sound.h"
#include "i_system.h"
#include "i_swap.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"
#include "z_zone.h"

#include "doomtype.h"

#define NUM_CHANNELS 16
#define NUM_MIDI_CHANNELS 16

int use_libsamplerate = 0;

// Scale factor used when converting libsamplerate floating point numbers
// to integers. Too high means the sounds can clip; too low means they
// will be too quiet. This is an amount that should avoid clipping most
// of the time: with all the Doom IWAD sound effects, at least. If a PWAD
// is used, clipping might occur.

float libsamplerate_scale = 0.65f;

static void I_OGB_PrecacheSounds(sfxinfo_t *sounds, int num_sounds){
	printf("%cs: %d\n", __func__, num_sounds);
}

//
// Retrieve the raw data lump index
//  for a given SFX name.
//

static int I_OGB_GetSfxLumpNum(sfxinfo_t *sfx){
	printf("%cs\n", __func__);
	return 0;
}

static void I_OGB_UpdateSoundParams(int handle, int vol, int sep){
	printf("%cs\n: vol: %d, sep: %d\n", __func__, vol, sep);
}

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.
//
static int I_OGB_StartSound(sfxinfo_t *sfxinfo, int channel, int vol, int sep){
	printf("%cs: vol: %d, sep: %d\n", __func__, vol, sep);
	return 1;
}

static void I_OGB_StopSound(int handle){
	printf("%cs\n", __func__);
}

static bool I_OGB_SoundIsPlaying(int handle){
	printf("%cs\n", __func__);
	return false;
}

// 
// Periodically called to update the sound system
//
static void I_OGB_UpdateSound(void){
	printf("%cs\n", __func__);
}


static void I_OGB_ShutdownSound(void)
{
	printf("%cs\n", __func__);
}


static bool I_OGB_InitSound(bool _use_sfx_prefix)
{
	printf("%cs: %d\n", __func__, _use_sfx_prefix);
	return true;
}

static snddevice_t sound_OGB_devices[] =
{
	SNDDEVICE_SB,
	SNDDEVICE_PAS,
	SNDDEVICE_GUS,
	SNDDEVICE_WAVEBLASTER,
	SNDDEVICE_SOUNDCANVAS,
	SNDDEVICE_AWE32,
};

sound_module_t DG_sound_module = 
{
	sound_OGB_devices,
	arrlen(sound_OGB_devices),
	I_OGB_InitSound,
	I_OGB_ShutdownSound,
	I_OGB_GetSfxLumpNum,
	I_OGB_UpdateSound,
	I_OGB_UpdateSoundParams,
	I_OGB_StartSound,
	I_OGB_StopSound,
	I_OGB_SoundIsPlaying,
	I_OGB_PrecacheSounds,
};

