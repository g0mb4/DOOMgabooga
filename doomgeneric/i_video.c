// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
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
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include "v_video.h"
#include "m_argv.h"
#include "d_event.h"
#include "d_main.h"
#include "i_video.h"
#include "z_zone.h"

#include "tables.h"
#include "doomkeys.h"

#include "doomgeneric.h"

extern void I_InitInput(void);

int usemouse = 0;

static struct color colors[256];

void I_GetEvent(void);

// The screen buffer; this is modified to draw things to the screen

byte *I_VideoBuffer = NULL;

// If true, game is running as a screensaver

boolean screensaver_mode = false;

// Flag indicating whether the screen is currently visible:
// when the screen isnt visible, don't render the screen

boolean screenvisible;

// Mouse acceleration
//
// This emulates some of the behavior of DOS mouse drivers by increasing
// the speed when the mouse is moved fast.
//
// The mouse input values are input directly to the game, but when
// the values exceed the value of mouse_threshold, they are multiplied
// by mouse_acceleration to increase the speed.

float mouse_acceleration = 2.0;
int mouse_threshold = 10;

// Gamma correction level to use

int usegamma = 0;

typedef struct
{
	byte r;
	byte g;
	byte b;
} col_t;

static uint16_t rgb565_palette[256];

void colormap_to_pixel(u32 * out, u8 * in, int in_pixels)
{
    int i, j, k;
    struct color c;
    uint32_t pix;

    for (i = 0; i < in_pixels; i++)
    {
        c = colors[*in];  /* R:8 G:8 B:8 format! */
        pix = c.r;
        pix |= c.g << 8;
        pix |= c.b << 16;
        pix |= 0xFF << 24;

        *out++ = pix;
        in++;
    }
}

void I_InitGraphics (void)
{
    int i;

    printf("I_InitGraphics: DOOM screen size: w x h: %d x %d\n", SCREENWIDTH, SCREENHEIGHT);

    /* Allocate screen to draw to */
	I_VideoBuffer = (byte*)Z_Malloc (SCREENWIDTH * SCREENHEIGHT, PU_STATIC, NULL);  // For DOOM to draw on

	screenvisible = true;

    I_InitInput();
}

void I_ShutdownGraphics (void)
{
	Z_Free (I_VideoBuffer);
}

void I_StartFrame (void)
{

}

void I_StartTic (void)
{
	I_GetEvent();
}

void I_UpdateNoBlit (void)
{
}

//
// I_FinishUpdate
//

void I_FinishUpdate (void)
{
    /* DRAW SCREEN */
    u8 *line_in  = (u8 *) I_VideoBuffer + (SCREENWIDTH * SCREENHEIGHT) - SCREENWIDTH;
    u32 *line_out = (u32 *) DG_ScreenBuffer;

    int y = SCREENHEIGHT;

    // flip y
    while (y--){
        colormap_to_pixel((void*)line_out, (void*)line_in, SCREENWIDTH);
        line_out += SCREENWIDTH;
        line_in -= SCREENWIDTH;
    }

	DG_DrawFrame();
}

//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT);
}

//
// I_SetPalette
//
#define GFX_RGB565(r, g, b)			((((r & 0xF8) >> 3) << 11) | (((g & 0xFC) >> 2) << 5) | ((b & 0xF8) >> 3))
#define GFX_RGB565_R(color)			((0xF800 & color) >> 11)
#define GFX_RGB565_G(color)			((0x07E0 & color) >> 5)
#define GFX_RGB565_B(color)			(0x001F & color)

void I_SetPalette (byte* palette)
{
	int i;
	//col_t* c;

	//for (i = 0; i < 256; i++)
	//{
	//	c = (col_t*)palette;

	//	rgb565_palette[i] = GFX_RGB565(gammatable[usegamma][c->r],
	//								   gammatable[usegamma][c->g],
	//								   gammatable[usegamma][c->b]);

	//	palette += 3;
	//}
    

    /* performance boost:
     * map to the right pixel format over here! */

    for (i=0; i<256; ++i ) {
        colors[i].a = 0;
        colors[i].r = gammatable[usegamma][*palette++];
        colors[i].g = gammatable[usegamma][*palette++];
        colors[i].b = gammatable[usegamma][*palette++];
    }

}

// Given an RGB value, find the closest matching palette index.

int I_GetPaletteIndex (int r, int g, int b)
{
    int best, best_diff, diff;
    int i;
    col_t color;

    printf("I_GetPaletteIndex\n");

    best = 0;
    best_diff = INT_MAX;

    for (i = 0; i < 256; ++i)
    {
    	color.r = GFX_RGB565_R(rgb565_palette[i]);
    	color.g = GFX_RGB565_G(rgb565_palette[i]);
    	color.b = GFX_RGB565_B(rgb565_palette[i]);

        diff = (r - color.r) * (r - color.r)
             + (g - color.g) * (g - color.g)
             + (b - color.b) * (b - color.b);

        if (diff < best_diff)
        {
            best = i;
            best_diff = diff;
        }

        if (diff == 0)
        {
            break;
        }
    }

    return best;
}

void I_BeginRead (void)
{
}

void I_EndRead (void)
{
}

void I_SetWindowTitle (char *title)
{
	DG_SetWindowTitle(title);
}

void I_GraphicsCheckCommandLine (void)
{
}

void I_SetGrabMouseCallback (grabmouse_callback_t func)
{
}

void I_EnableLoadingDisk(void)
{
}

void I_BindVideoVariables (void)
{
}

void I_DisplayFPSDots (boolean dots_on)
{
}

void I_CheckIsScreensaver (void)
{
}
