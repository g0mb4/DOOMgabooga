
#include "m_argv.h"

#include "doomgeneric.h"

pixel_t* DG_ScreenBuffer = NULL;

void M_FindResponseFile(void);
void D_DoomMain (void);


void doomgeneric_Create(int argc, char **argv)
{
	// save arguments
    myargc = argc;
    myargv = argv;

	M_FindResponseFile();

	DG_ScreenBuffer = alloc(get_heap_allocator(), SCREENWIDTH * SCREENHEIGHT * sizeof(pixel_t));
	assert(DG_ScreenBuffer, "Unbale to create DG_ScreenBuffer");

	DG_Init();

	D_DoomMain ();
}

