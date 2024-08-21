caveman's port of DOOM

engine: https://github.com/alpinestudios/oogabooga

DOOM: https://github.com/ozkl/doomgeneric

## note
It needs a .wad file in the root directory (the shareware (DOOM1.WAD) does the job).

Sound support is work in progress, comment `#define FEATURE_SOUND` in *doomgeneric/doomfeatures.h* to disable it and get a bit more stable version.

No multiplayer.

## TODO
+ implement music (play MIDI)
+ general cleanup
+ remove `ORIGCODE` + other unused `#define`-s
+ fix warnings
+ fix `TODO(gmb)`s
