

///
// Build config stuff

#define INITIAL_PROGRAM_MEMORY_SIZE MB(5)

// You might want to increase this if you get a log warning saying the temporary
// storage was overflown. In many cases, overflowing the temporary storage
// should be fine since it just wraps back around and allocations made way
// earlier in the frame are likely not used anymore. This might however not
// always be the case, so it's probably a good idea to make sure you always have
// enough temporary storage for your game.
#define TEMPORARY_STORAGE_SIZE MB(2)

// Enable VERY_DEBUG if you are having memory bugs to detect things like heap
// corruption earlier. #define VERY_DEBUG 1

typedef struct Context_Extra {
  int monkee;
} Context_Extra;
// This needs to be defined before oogabooga if we want extra stuff in context
#define CONTEXT_EXTRA Context_Extra

// This defaults to "entry", but we can set it to anything (except "main" or
// other existing proc names"
#define ENTRY_PROC entry

// Ooga booga needs to be included AFTER configuration and BEFORE the program
// code


#include "oogabooga/oogabooga.c"

#include "doomgeneric/oogabooga_helpers.c"
#include "entry_doomgabooga.c"

#include "doomgeneric/am_map.c"
#include "doomgeneric/d_event.c"
#include "doomgeneric/d_items.c"
#include "doomgeneric/d_iwad.c"
#include "doomgeneric/d_loop.c"
#include "doomgeneric/d_main.c"
#include "doomgeneric/d_mode.c"
#include "doomgeneric/d_net.c"
#include "doomgeneric/doomdef.c"
#include "doomgeneric/doomgeneric.c"
#include "doomgeneric/doomstat.c"
#include "doomgeneric/dstrings.c"
#include "doomgeneric/dummy.c"
#include "doomgeneric/f_finale.c"
#include "doomgeneric/f_wipe.c"
#include "doomgeneric/g_game.c"
#include "doomgeneric/gusconf.c"
#include "doomgeneric/hu_lib.c"
#include "doomgeneric/hu_stuff.c"
#include "doomgeneric/i_cdmus.c"
#include "doomgeneric/i_endoom.c"
#include "doomgeneric/i_input.c"
#include "doomgeneric/i_joystick.c"
#include "doomgeneric/i_scale.c"
#include "doomgeneric/i_sound.c"
#include "doomgeneric/i_oogabooga_sound.c"
#include "doomgeneric/i_oogabooga_music.c"
#include "doomgeneric/i_system.c"
#include "doomgeneric/i_timer.c"
#include "doomgeneric/i_video.c"
#include "doomgeneric/icon.c"
#include "doomgeneric/info.c"
#include "doomgeneric/m_argv.c"
#include "doomgeneric/m_bbox.c"
#include "doomgeneric/m_cheat.c"
#include "doomgeneric/m_config.c"
#include "doomgeneric/m_controls.c"
#include "doomgeneric/m_fixed.c"
#include "doomgeneric/m_menu.c"
#include "doomgeneric/m_misc.c"
#include "doomgeneric/m_random.c"
#include "doomgeneric/memio.c"
#include "doomgeneric/mus2mid.c"
#include "doomgeneric/p_ceilng.c"
#include "doomgeneric/p_doors.c"
#include "doomgeneric/p_enemy.c"
#include "doomgeneric/p_floor.c"
#include "doomgeneric/p_inter.c"
#include "doomgeneric/p_lights.c"
#include "doomgeneric/p_map.c"
#include "doomgeneric/p_maputl.c"
#include "doomgeneric/p_mobj.c"
#include "doomgeneric/p_plats.c"
#include "doomgeneric/p_pspr.c"
#include "doomgeneric/p_saveg.c"
#include "doomgeneric/p_setup.c"
#include "doomgeneric/p_sight.c"
#include "doomgeneric/p_spec.c"
#include "doomgeneric/p_switch.c"
#include "doomgeneric/p_telept.c"
#include "doomgeneric/p_tick.c"
#include "doomgeneric/p_user.c"
#include "doomgeneric/r_bsp.c"
#include "doomgeneric/r_data.c"
#include "doomgeneric/r_draw.c"
#include "doomgeneric/r_main.c"
#include "doomgeneric/r_plane.c"
#include "doomgeneric/r_segs.c"
#include "doomgeneric/r_sky.c"
#include "doomgeneric/r_things.c"
#include "doomgeneric/s_sound.c"
#include "doomgeneric/sha1.c"
#include "doomgeneric/sounds.c"
#include "doomgeneric/st_lib.c"
#include "doomgeneric/st_stuff.c"
#include "doomgeneric/statdump.c"
#include "doomgeneric/tables.c"
#include "doomgeneric/v_video.c"
#include "doomgeneric/w_checksum.c"
#include "doomgeneric/w_file.c"
#include "doomgeneric/w_file_stdc.c"
#include "doomgeneric/w_main.c"
#include "doomgeneric/w_wad.c"
#include "doomgeneric/wi_stuff.c"
#include "doomgeneric/z_zone.c"    
          