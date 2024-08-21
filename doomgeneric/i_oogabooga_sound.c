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

#define NUM_SND_CHANNELS 16

static sfxinfo_t *channels_playing[NUM_SND_CHANNELS];
static Audio_Player *audio_players[NUM_SND_CHANNELS];

static bool use_sfx_prefix;

int use_libsamplerate = 0;

// Scale factor used when converting libsamplerate floating point numbers
// to integers. Too high means the sounds can clip; too low means they
// will be too quiet. This is an amount that should avoid clipping most
// of the time: with all the Doom IWAD sound effects, at least. If a PWAD
// is used, clipping might occur.

float libsamplerate_scale = 0.65f;

// 0 - 127 -> 0.0 - 1.0
static f32 set_volume(int vol){
	return ((f32)(vol) / 127.0);
}

static void destroy_channel_and_audio_source(int channel){
	if (channel < 0 || channel >= NUM_SND_CHANNELS){
		return;
	}

	if (channels_playing[channel]) {
		Audio_Source *src = (Audio_Source *)channels_playing[channel]->driver_data;

		if (src){
			//audio_source_destroy(src);
		}

		channels_playing[channel] = NULL;
	}
}

/*
	unsigned 8 bits --> signed 16 bits
	mono --> stereo
*/
static void convert_to_pcm(Audio_Source *src, u8 *data, int len){
	src->pcm_frames = alloc(src->allocator, len * sizeof(s16) * 2);
	assert(src->pcm_frames, "Unable to allocat PCM frames.");

	u32 f = 0;
	s16 *pcm_s16 = (s16 *)src->pcm_frames;
	for(int i = 0; i < len; ++i){
		const f32 scale = (f32)data[i] / 127.5 - 1;
		const s16 val = (s16)((f32)INT16_MAX * scale);
		pcm_s16[f++] = val;	// left
		pcm_s16[f++] = val;	// right
	}
}

static Audio_Source* create_audio_source(void* data, int len, int sample_rate){
	Audio_Source *src = alloc(get_heap_allocator(), sizeof(Audio_Source));
	assert(src, "Unable to create source.");

	src->kind = AUDIO_SOURCE_MEMORY;
	src->format = (Audio_Format){
		.bit_width = AUDIO_BITS_16,
		.channels = 2,
		.sample_rate = sample_rate,
	};
	src->allocator = get_heap_allocator();
	src->number_of_frames = len;
	src->uid = next_audio_source_uid++;

	mutex_init(&src->mutex_for_destroy);

	convert_to_pcm(src, (u8*)data, len);
	return src;
}

// Load and convert a sound effect
// Returns true if successful

static bool CacheSFX(sfxinfo_t *sfxinfo){
	int lumpnum;
	u32 lumplen;
	int samplerate;
	u32 length;
	u8 *data;

	// need to load the sound

	lumpnum = sfxinfo->lumpnum;
	data = W_CacheLumpNum(lumpnum, PU_STATIC);
	lumplen = W_LumpLength(lumpnum);

	// Check the header, and ensure this is a valid sound

	if (lumplen < 8 || data[0] != 0x03 || data[1] != 0x00) {
		// Invalid sound
		return false;
	}

	// 16 bit sample rate field, 32 bit length field
	samplerate = (data[3] << 8) | data[2];
	length = (data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4];

	// If the header specifies that the length of the sound is greater than
	// the length of the lump itself, this is an invalid sound lump

	// We also discard sound lumps that are less than 49 samples long,
	// as this is how DMX behaves - although the actual cut-off length
	// seems to vary slightly depending on the sample rate.  This needs
	// further investigation to better understand the correct
	// behavior.

	if (length > lumplen - 8 || length <= 48){
		return false;
	}

	// The DMX sound library seems to skip the first 16 and last 16
	// bytes of the lump - reason unknown.

	data += 16;
	length -= 32;

	Audio_Source *src = create_audio_source(data, length, samplerate);

	if(sfxinfo->driver_data){
		dealloc(get_heap_allocator(), sfxinfo->driver_data);
	}

	sfxinfo->driver_data = src;

	// don't need the original lump any more

	W_ReleaseLumpNum(lumpnum);

	return true;
}

static void GetSfxLumpName(sfxinfo_t *sfx, char *buf, size_t buf_len)
{
    // Linked sfx lumps? Get the lump number for the sound linked to.

    if (sfx->link != NULL){
        sfx = sfx->link;
    }

    // Doom adds a DS* prefix to sound lumps; Heretic and Hexen don't
    // do this.

    if (use_sfx_prefix){
        M_snprintf(buf, buf_len, "ds%s", DEH_String(sfx->name));
    } else {
        M_StringCopy(buf, DEH_String(sfx->name), buf_len);
    }
}

static void I_OGB_PrecacheSounds(sfxinfo_t *sounds, int num_sounds){
	char namebuf[9];
	int i;

	printf("I_OGB_PrecacheSounds: Precaching all sound effects..");

	for (int i = 0; i < num_sounds; ++i){
		if ((i % 6) == 0){
			printf(".");
		}

		GetSfxLumpName(&sounds[i], namebuf, sizeof(namebuf));

		sounds[i].lumpnum = W_CheckNumForName(namebuf);

		if (sounds[i].lumpnum != -1){
			CacheSFX(&sounds[i]);
		}
	}

	printf("\n");
}

//
// Retrieve the raw data lump index
//  for a given SFX name.
//

static int I_OGB_GetSfxLumpNum(sfxinfo_t *sfx){
	char namebuf[9];

	GetSfxLumpName(sfx, namebuf, sizeof(namebuf));

	return W_GetNumForName(namebuf);
}

static void I_OGB_UpdateSoundParams(int handle, int vol, int sep){
	if (handle < 0 || handle >= NUM_SND_CHANNELS){
		return;
	}

	audio_players[handle]->config.volume = set_volume(vol);
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
	if (channel < 0 || channel >= NUM_SND_CHANNELS){
		return -1;
	}

	// Release a sound effect if there is already one playing
	// on this channel
	destroy_channel_and_audio_source(channel);

	// Get the sound data
	if (sfxinfo->driver_data == NULL){
		if (!CacheSFX(sfxinfo)){
			return -1;
		}
	}
	assert(sfxinfo->driver_data, "Invalid SFX driver data");

	Audio_Source * src = (Audio_Source*)sfxinfo->driver_data;

	audio_players[channel]->config.volume = set_volume(vol);
	audio_players[channel]->config.playback_speed = 1.0;

	audio_players[channel] = audio_player_get_one();
	assert(audio_players[channel], "Unable to get an audio player");

	audio_player_set_source(audio_players[channel], *src);
	audio_player_set_state(audio_players[channel], AUDIO_PLAYER_STATE_PLAYING);
	audio_players[channel]->release_when_done = true;

	channels_playing[channel] = sfxinfo;
	return channel;
}

static void I_OGB_StopSound(int handle){
	if (handle < 0 || handle >= NUM_SND_CHANNELS){
		return;
	}

	audio_player_set_state(audio_players[handle], AUDIO_PLAYER_STATE_PAUSED);
}

static bool I_OGB_SoundIsPlaying(int handle){
	if (handle < 0 || handle >= NUM_SND_CHANNELS){
		return false;
	}

	return audio_players[handle]->allocated;
}

// 
// Periodically called to update the sound system
//
static void I_OGB_UpdateSound(void){
	for (int i = 0; i < NUM_SND_CHANNELS; ++i) {
		if (channels_playing[i] && !audio_players[i]->allocated) {
			destroy_channel_and_audio_source(i);
		}
	}
}

static void I_OGB_ShutdownSound(void){
	// NOTE(gmb): OS will handle this
}

static bool I_OGB_InitSound(bool _use_sfx_prefix){
	use_sfx_prefix = _use_sfx_prefix;

	for (int i = 0; i < NUM_SND_CHANNELS; ++i) {
		channels_playing[i] = NULL;

		// NOTE(gmb): Not sure this is a right way to init the array.
		audio_players[i] = audio_player_get_one();
		assert(audio_players[i], "Unable to get an audio player");
		
		audio_player_set_state(audio_players[i], AUDIO_PLAYER_STATE_PAUSED);
		audio_players[i]->marked_for_release = true;
	}

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
